/*
 * satellite_service.h
 *
 *  Created on: 1 abr. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_SATELLITE_SERVICE_H_
#define SRC_SATELLITE_SERVICE_H_
typedef struct sat_telemetry
{
	int satellite_id;
    float uptime;
	short firm_version;
    char mem_usage;
    char cpu_usage;
} sat_telemetry;

int init_sat_service();
int upgrade_firmware(char* file_name);
int earth_surface_scan();
int get_telemetry_data(sat_telemetry* telemetry_data);
int stop_sat_service();

#endif /* SRC_SATELLITE_SERVICE_H_ */
