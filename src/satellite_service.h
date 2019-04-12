/*
 * satellite_service.h
 *
 *  Created on: 1 abr. 2019
 *      Author: steve-urbit
 */

#ifndef SRC_SATELLITE_SERVICE_H_
#define SRC_SATELLITE_SERVICE_H_
typedef struct update_params{
	int update_version;
	//char* file_name;
	long file_size_bytes;
} update_params;

typedef struct update_result{
	int current_version;
} update_result;
/*
typedef struct scan_params{
	int update_version;
} scan_params;
*/
#define SLICE_META_FMT "{name:%Q,size:%lu}"
typedef struct slice_meta{
	char* slice_name;
    long slice_size_bytes;
} slice_meta;

typedef struct scan_result{
	int slices_quantity;
    slice_meta* slices_dataset;
} scan_result;
/*
typedef struct telemetry_params{
	int update_version;
} telemetry_params;
*/
typedef struct rpc_error{
	int error_code;
    char* error_message;
} rpc_error;



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
