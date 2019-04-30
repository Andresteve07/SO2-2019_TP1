#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "both.h"
#include "modulo.h"
#include "fff.h"
#include "frozen.h"
#include "satellite_service.h"
#include "log.h"
#include <regex.h>

DEFINE_FFF_GLOBALS
#define TEST_F(SUITE, NAME) void NAME()
#define RUN_TEST(SUITE, TESTNAME) printf(" Running %s.%s: \n", #SUITE, #TESTNAME); setup(); TESTNAME(); printf(" SUCCESS\n");

FAKE_VALUE_FUNC1(int, modulo, int);

void setup()
{
	RESET_FAKE(modulo);
}

TEST_F(JSONTest, regex_test){
	regex_t regex;
int reti;
char msgbuf[100];

/* Compile regular expression */
reti = regcomp(&regex, "[[:alnum:]{3}]", 0);
if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
}

/* Execute regular expression */
reti = regexec(&regex, "abc", 0, NULL, 0);
if (!reti) {
    puts("Match");
}
else if (reti == REG_NOMATCH) {
    puts("No match");
}
else {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    exit(1);
}

/* Free memory allocated to the pattern buffer by regcomp() */
regfree(&regex);
}

TEST_F(JSONTest, json_file_search_test){
	char* users_json;
	
	users_json = json_fread("../assets/users_db.json");

	int i, len = strlen(users_json);
	log_trace("JSON: %s",users_json);
	char* obj_user_name;
	char* obj_password;
  	struct json_token t;
	
	for (i = 0; json_scanf_array_elem(users_json, len, ".users", i, &t) > 0; i++) {
		// t.type == JSON_TYPE_OBJECT
		json_scanf(t.ptr, t.len, "{user_name: %Q,password: %Q}", &obj_user_name, &obj_password);
		log_trace("NAME: %s, PASS: %s",obj_user_name,obj_password);
	}
}
TEST_F(ModuloTest, easy_json_test){
	char* str = "{ \"a\": 123, \"b\": \"hi\", \"c\": false }";
	int value = 0;
	char* b;
	json_scanf(str, strlen(str), "{a: %d}", &value);
	printf("value %i\n", value);
	assert(value==123);

}

TEST_F(JSONTest, print_qfield_json_test){
	char* json_str = "{ \"a\": 123, \"c\": false }";
	char rpc_buf[200];
	struct json_out output = JSON_OUT_BUF(rpc_buf, 200);
	int result = json_printf(& output, "{str:%Q}",json_str);
	printf("print_result:%i, rpc_buf:%s\n",result, rpc_buf);
	assert(strlen(rpc_buf)==38);
}

TEST_F(JSONTest, qfield_json_test){
	
	char* str = "{ \"a\": 123, \"payload\":\"{\\\"cpu_usage\\\":73.68,\\\"firmware_version\\\":12413,\\\"mem_usage\\\":45.71,\\\"satellite_id\\\":555555,\\\"uptime\\\":98.89}\", \"c\": false }";
	char* b;
	int result = json_scanf(str, strlen(str), "{payload: %Q}", &b);
	printf("result:%i, b:%s\n",result, b);
	assert(strlen(b)==99);
}

TEST_F(JSONTest, telemetry_json_scan_test){
	
	char* str = "{\"cpu_usage\":73.68,\"firmware_version\":12413,\"mem_usage\":45.71,\"satellite_id\":555555,\"uptime\":98.89}";
	char* fmt = "{cpu_usage:%f,firmware_version:%d,mem_usage:%f,satellite_id:%d,uptime:%f}";
	float c,m,u;
	int f,s;
	int result = json_scanf(str, strlen(str), fmt, &c,&f,&m,&s,&u);
	printf("result:%i, c:%f,m:%f,u:%f,f:%i,s:%i\n",result, c,m,u,f,s);
	//assert(strlen(fmt)==99);
}

static void scan_slice_array(const char* str, int length, void* user_data){
    char* slice_name;
    long slice_size;
    slice_meta slices_dataset[500];

    struct json_token token;
    for (int i = 0; json_scanf_array_elem(str, length, ".slices_dataset",i,& token) > 0; i++){
        slice_meta slice_data;
		log_trace("TOKEN: %p, %lu", token.ptr,token.len);
        json_scanf(token.ptr,token.len,SLICE_META_FMT, & slice_name,& slice_size);
		log_trace("PARSED NAME: %s SIZE: %lu",slice_name,slice_size);
        long bytes_for_name = strlen(slice_name) + 1;
        slice_data.slice_name=malloc(bytes_for_name);
        bzero(slice_data.slice_name,bytes_for_name);
        strcpy(slice_data.slice_name,slice_name);
        slice_data.slice_size_bytes = slice_size;
        slices_dataset[i] = slice_data;
    }
    user_data = slices_dataset;
}

TEST_F(JSONTest, json_scan_slices_test){
	
	char* str_json = "{\"slice_quantity\":3, \"slices_dataset\":[{\"name\":\"scann_4.jpg\",\"size\":79717}, {\"name\":\"scann_5.jpg\",\"size\":178845}, {\"name\":\"scann_6.jpg\",\"size\":200802}]}";
	char* fmt = "{slice_quantity:%d, slices_dataset:%M}";
	scan_result result;
    
    int fields_loaded = json_scanf(str_json, strlen(str_json),
    fmt,
    & result.slices_quantity,
    scan_slice_array,
    & result.slices_dataset);
	printf("reads:%i, Quantity:%i\n",fields_loaded, result.slices_quantity);
	for (int i = 0; i < 3; i++)
	{
		printf("SLICE(%i) - NAME: %s, SIZE: %lu",i,
		result.slices_dataset[i].slice_name,
		result.slices_dataset[i].slice_size_bytes);
	}
	
	//assert(strlen(fmt)==99);
}

TEST_F(JSONTest, json_scan_slices_test_bis){
	
	char* str_json = "{\"slice_quantity\":3, \"slices_dataset\":[{\"name\":\"scann_4.jpg\",\"size\":79717}, {\"name\":\"scann_5.jpg\",\"size\":178845}, {\"name\":\"scann_6.jpg\",\"size\":200802}]}";
	char* fmt = "{slice_quantity:%d, slices_dataset:%M}";
	scan_result result;
    
    int fields_loaded = json_scanf(str_json, strlen(str_json),
    "{slice_quantity:%d}",
    & result.slices_quantity);

	log_trace("QUANTITY: %i", result.slices_quantity);

	char* slice_name;
    long slice_size;
    slice_meta slices_dataset[500];
	long length = strlen(str_json);

    struct json_token token;
    for (int i = 0; json_scanf_array_elem(str_json, length, ".slices_dataset",i,& token) > 0; i++){
        slice_meta slice_data;
		log_trace("TOKEN: %c, %lu", *token.ptr,token.len);
        json_scanf(token.ptr,token.len,SLICE_META_FMT, & slice_name,& slice_size);
		log_trace("PARSED NAME: %s SIZE: %lu",slice_name,slice_size);
        long bytes_for_name = strlen(slice_name) + 1;
        slice_data.slice_name=malloc(bytes_for_name);
        bzero(slice_data.slice_name,bytes_for_name);
        strcpy(slice_data.slice_name,slice_name);
        slice_data.slice_size_bytes = slice_size;
        slices_dataset[i] = slice_data;
    }
	log_trace("ARRAY SIZE: %lu", sizeof(slices_dataset));
	
	//printf("reads:%i, Quantity:%i\n",fields_loaded, result.slices_quantity);
	
	for (int i = 0; i < 3; i++)
	{
		log_trace("SLICE(%i) - NAME: %s, SIZE: %lu",i,
		slices_dataset[i].slice_name,
		slices_dataset[i].slice_size_bytes);
	}
	
	
	assert(strlen(fmt)==99);
}

TEST_F(ModuloTest, both_even_numbers)
{
	int a;
	int b;
	int result;

	// Given
	a = 2;
	b = 4;
	modulo_fake.return_val_seq_len = 2;
	modulo_fake.return_val_seq = (int *) malloc(2*sizeof(int));
	modulo_fake.return_val_seq[0] = 0;
	modulo_fake.return_val_seq[1] = 0;

	// When
	result = both_even(a, b);
	printf("Valor %i\n", result);
	// Then
	assert(result == 1);
	assert(modulo_fake.call_count == 2);

}

TEST_F(ModuloTest, first_even_and_second_odd)
{
	int a;
	int b;
	int result;

	// Given
	a = 2;
	b = 3;
	modulo_fake.return_val_seq_len = 2;
	modulo_fake.return_val_seq = (int *) malloc(2*sizeof(int));
	modulo_fake.return_val_seq[0] = 0;
	modulo_fake.return_val_seq[1] = 1;

	// When
	result = both_even(a, b);

	// Then
	assert(result != 1);
}

TEST_F(ModuloTest, first_odd_and_second_even)
{
	int a;
	int b;
	int result;

	// Given
	a = 3;
	b = 2;
	modulo_fake.return_val_seq_len = 2;
	modulo_fake.return_val_seq = (int *) malloc(2*sizeof(int));
	modulo_fake.return_val_seq[0] = 1;
	modulo_fake.return_val_seq[1] = 0;

	// When
	result = both_even(a, b);

	// Then
	assert(result != 1);
}


TEST_F(ModuloTest, both_odd_numbers)
{
	int a;
	int b;
	int result;

	// Given
	a = 3;
	b = 5;
	modulo_fake.return_val_seq_len = 2;
	modulo_fake.return_val_seq = (int *) malloc(2*sizeof(int));
	modulo_fake.return_val_seq[0] = 1;
	modulo_fake.return_val_seq[1] = 1;

	// When
	result = both_even(a, b);

	// Then
	assert(result != 1);
}

TEST_F(ModuloTest, one_even_and_other_zero)
{
	int a;
	int b;
	int result;

	// Given
	a = 2;
	b = 0;
	modulo_fake.return_val_seq_len = 2;
	modulo_fake.return_val_seq = (int *) malloc(2*sizeof(int));
	modulo_fake.return_val_seq[0] = 0;
	modulo_fake.return_val_seq[1] = 0;

	// When
	result = both_even(a, b);

	// Then
	assert(result != 0);
}

TEST_F(ModuloTest, one_odd_and_other_zero)
{
	int a;
	int b;
	int result;

	// Given
	a = 0;
	b = 5;
	modulo_fake.return_val_seq_len = 2;
	modulo_fake.return_val_seq = (int *) malloc(2*sizeof(int));
	modulo_fake.return_val_seq[0] = 0;
	modulo_fake.return_val_seq[1] = 1;

	// When
	result = both_even(a, b);

	// Then
	assert(result!=1);
}

int main(void)
{
	log_set_level(LOG_TRACE);
	log_set_quiet(0);

	RUN_TEST(JSONTest, regex_test);
	RUN_TEST(JSON_Test, json_file_search_test);
	RUN_TEST(JSONTest, json_scan_slices_test_bis);
	RUN_TEST(JSONTest, telemetry_json_scan_test);
	RUN_TEST(ModuloTest, easy_json_test);
	RUN_TEST(JSONTest, qfield_json_test);
	RUN_TEST(JSONTest, print_qfield_json_test);
	RUN_TEST(ModuloTest, both_odd_numbers);
	RUN_TEST(ModuloTest, both_even_numbers);
	RUN_TEST(ModuloTest, first_even_and_second_odd);
	RUN_TEST(ModuloTest, first_odd_and_second_even);
	RUN_TEST(ModuloTest, one_odd_and_other_zero);
	RUN_TEST(ModuloTest, one_even_and_other_zero);

	return 0;
}
