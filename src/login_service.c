/*
 * login_service.c
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */
#include "login_service.h"
#include "log.h"
#include "frozen.h"
#include "string.h"
#include <stdlib.h>
#include <stdbool.h>

bool user_name_format_check(char* user_name){
	return strlen(user_name)>2;
}

bool password_format_check(char* password){
	return strlen(password)>2;
}

int credential_validation(char* user_name,char* password){
	if(!user_name_format_check(user_name)){
		log_trace("Invalid user name format.");
		return FAILURE;
	}
	if(!password_format_check(user_name)){
		log_trace("Invalid password format.");
		return FAILURE;
	}
	char* users_json;
	
	users_json = json_fread("../assets/users_db.json");

	int i, len = strlen(users_json);
	log_trace("JSON: %s",users_json);
	char* obj_user_name;
	char* obj_password;
  	struct json_token t;
	
	for (i = 0; json_scanf_array_elem(users_json, len, ".users", i, &t) > 0; i++) {
		json_scanf(t.ptr, t.len, "{user_name: %Q,password: %Q}", &obj_user_name, &obj_password);
		log_trace("NAME: %s, PASS: %s",obj_user_name,obj_password);
		if (strcmp(user_name, obj_user_name) == 0 
			&& strcmp(password, obj_password) == 0){
			log_trace("User credentials match!");
			return SUCCESS;
		}
	}
	free(users_json);
	return FAILURE;
}

login_result login(char* user_name,char* password){
	return credential_validation(user_name,password);
}

