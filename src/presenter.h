/*
 * presenter.h
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_PRESENTER_H_
#define SRC_PRESENTER_H_

int login_try_counter;
void init_presenter(char* program_params[], int params_count);
void process_login_data(char* user_name, char* user_password);
void process_option_selection(int selected_option);
void start_firmware_update(char* upgrade_file_name);
void start_image_scanning();
void request_sat_telemetry();

#endif /* SRC_PRESENTER_H_ */
