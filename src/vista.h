/*
 * vista.h
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_VISTA_H_
#define SRC_VISTA_H_

void init_vista(char* program_params[], int params_count);
void show_program_help(char* program_name);
void show_login_promt();
void show_login_failure_message(int tryCount);
void show_login_success_message();
void show_command_promt();
void show_firmware_update_prompt();
void show_upgrade_success();
void show_upgrade_failure();
void show_scanning_started();
void show_scan_success_message();
void show_scan_failure_message();
void show_telemetry_promt();
void show_goodby_message();
void show_telemetry_data(float cpu_usage,int firm_version,float mem_usage,int satellite_id,float uptime);
void show_telemetry_failure();

#endif /* SRC_VISTA_H_ */
