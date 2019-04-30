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

void init_presenter(char* program_params[], int params_count){
	log_debug("PRESENTER INIT\n");
	if (params_count < 2) {
		show_program_help(program_params[0]);
		log_error("Program params missing.");
		exit(0);
	}
	init_sat_service(program_params[1]);
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
	if(upgrade_firmware("sadas")==0){
		show_upgrade_success();
	} else {
		show_upgrade_failure();
	}
}

void start_image_scanning(){
	if(earth_surface_scan()==0){
		show_scan_success_message();
	} else {
		show_scan_failure_message();
	}
}

void request_sat_telemetry(){
	sat_telemetry telemetry_data;
	//malloc(sizeof(sat_telemetry));
	//("cpu_usage:%f,firmware_version:%i,mem_usage:%f,satellite_id:%i,uptime:%f",
        
	if(get_telemetry_data(&telemetry_data)==0){
		show_telemetry_data(telemetry_data.cpu_usage,
        telemetry_data.firm_version,
        telemetry_data.mem_usage,
        telemetry_data.satellite_id,
        telemetry_data.uptime);
	} else {
		show_telemetry_failure();
	}
}