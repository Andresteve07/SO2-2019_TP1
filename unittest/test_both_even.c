#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "both.h"
#include "modulo.h"
#include "fff.h"
#include "frozen.h"

DEFINE_FFF_GLOBALS
#define TEST_F(SUITE, NAME) void NAME()
#define RUN_TEST(SUITE, TESTNAME) printf(" Running %s.%s: \n", #SUITE, #TESTNAME); setup(); TESTNAME(); printf(" SUCCESS\n");

FAKE_VALUE_FUNC1(int, modulo, int);

void setup()
{
	RESET_FAKE(modulo);
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
