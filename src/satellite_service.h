/*
 * satellite_service.h
 *
 *  Created on: 1 abr. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_SATELLITE_SERVICE_H_
#define SRC_SATELLITE_SERVICE_H_
typedef struct update_response
{
	int current_version;
} update_response;

typedef struct sat_telemetry
{
	int satellite_id;
    float uptime;
	int firm_version;
    float mem_usage;
    float cpu_usage;
} sat_telemetry;

typedef struct scan_response
{
	int slices_quantity;
    int slice_size_bytes;
} scan_response;


int init_sat_service();
int upgrade_firmware(char* file_name);
int earth_surface_scan();
int get_telemetry_data(sat_telemetry* telemetry_data);
int stop_sat_service();

#endif /* SRC_SATELLITE_SERVICE_H_ */
