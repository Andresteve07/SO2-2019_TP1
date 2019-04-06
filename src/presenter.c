/*
 * presenter.c
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */


#include "vista.h"
#include "presenter.h"
#include "login_service.h"
#include "satellite_service.h"
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

void init_presenter(){
	log_debug("PRESENTER INIT\n");
	init_sat_service();
	show_login_promt();
}

void process_login_data(char* user_name, char* user_password){
	switch (login(user_name,user_password)) {
		case SUCCESS:
			show_login_success_message();
			login_try_counter = 0;
			show_command_promt();
			break;
		case FAILURE:
			login_try_counter++;
			show_login_failure_message(login_try_counter);
			if (login_try_counter<3) {
				show_login_promt();
			} else {
				exit(1);
			}
			break;
		default:
			break;
	}
}

void process_option_selection(int selected_option){
	switch (selected_option) {
		case 1:
			show_firmware_update_prompt();
			break;
		case 2:
			show_scanning_started();
			start_image_scanning();
			break;
		case 3:
			request_sat_telemetry();
			show_telemetry_promt();
			break;
		case 4:
			show_goodby_message();
			exit(0);
			break;
		default:
			break;
	}
	show_command_promt();
}

void start_firmware_update(char* upgrade_file_name){
	upgrade_firmware("sadas");
}

void start_image_scanning(){
	earth_surface_scan();
}

void request_sat_telemetry(){
	sat_telemetry telemetry_data;
	malloc(sizeof(sat_telemetry));
	get_telemetry_data(&telemetry_data);
}