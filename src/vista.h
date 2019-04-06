/*
 * vista.h
 *
 *  Created on: 30 mar. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_VISTA_H_
#define SRC_VISTA_H_

void init_vista();
void show_login_promt();
void show_login_failure_message(int tryCount);
void show_login_success_message();
void show_command_promt();
void show_firmware_update_prompt();
void show_scanning_started();
void show_telemetry_promt();
void show_goodby_message();

#endif /* SRC_VISTA_H_ */
