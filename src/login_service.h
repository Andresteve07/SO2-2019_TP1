/*
 * login_service.h
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_LOGIN_SERVICE_H_
#define SRC_LOGIN_SERVICE_H_

typedef enum{
	SUCCESS,
	FAILURE,
	UNAVAILABLE
}login_result;

login_result login(char* user_name,char* password);

#endif /* SRC_LOGIN_SERVICE_H_ */
