/*
 * vista.c
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */
#include "vista.h"
#include "presenter.h"
#include <stdio.h>
//#include <ncurses.h>
#include "log.h"
#define clear() //printf("\033[H\033[J")

void init_vista(){
	log_debug("VISTA INIT\n");
	//initscr();
	init_presenter();
}

void show_login_promt(){
	//clear();
	//refresh();
	//printw("holo%i",5);
	clear();
	char user_name_input[10];
	char user_password[10];
	printf("Login to operate earth station.\n");
	printf("User Name: ");
	scanf(" %s",user_name_input);
	user_name_input[9]='\0';
	printf("User Password: ");
	scanf(" %s",user_password);
	user_password[9]='\0';
	//printf("user %s, password %s\n",user_name_input, user_password);
	process_login_data(user_name_input, user_password);
}
void show_login_failure_message(int tryCount){
	printf("%i login attempt failed\n",tryCount);
}
void show_login_success_message(){
	printf("Login Success!\n");
}
void show_command_promt(){
	int selected_option;
	clear();
	printf("Select option:\n");
	printf("1.%s\n2.%s\n3.%s\n4.%s\n",
			"upgrade satellite firmware.",
			"start scanner.",
			"get telemetry data.",
			"salir del programa.");
	scanf("%i",&selected_option);
	process_option_selection(selected_option);
}

void show_firmware_update_prompt(){
	char upgrade_file_name[20];
	printf("Binary file name:  ");
	scanf("%s",upgrade_file_name);
	start_firmware_update(upgrade_file_name);
}
void show_scanning_started(){
	printf("Earth Scan Started.\n");
}
void show_telemetry_promt(){
	printf("Telemetry request sent.\n");
}

void show_goodby_message(){
	printf("Thank you for using SatStation.\n");
}