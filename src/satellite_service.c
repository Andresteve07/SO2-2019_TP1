#include <stdio.h>
#include "satellite_service.h"
#include "socket_client.h"
#include "frozen.h"
#include "log.h"
#include "string.h"

#define TELEMETRY_JSON_FMT "{cpu_usage:%f,firmware_version:%d,mem_usage:%f,satellite_id:%d,uptime:%f}"
#define SCAN_RESPONSE_JSON_FMT "{slice_quantity:%d,slice_size_bytes:%d}"
#define UPDATE_RESPONSE_JSON_FMT "{current_version:%d}"
#define UPDATE_COMMAND_CODE 1

#define TELEMETY_COMMAND_CODE 3

#define SCAN_COMMAND_CODE 2
#define FIRMWARE_COMMAND_CODE 1

#define STATION_ID 123456
#define SATELLITE_ID 987654

#define FIRMWARE_FILE_PATH "../assets/file_server"

int init_sat_service(){
    tcp_init();
    tcp_connect_to_server("sad");
}

int upgrade_firmware(char* file_name){
    rpc update_request = {
        UPDATE_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        NULL};
    rpc firmware_update_response;
    tcp_send_rpc_request(& update_request);
    tcp_recv_rpc_response(& firmware_update_response);
    update_response update_preconditions;
    int result = json_scanf(firmware_update_response.payload, strlen(firmware_update_response.payload),
    UPDATE_RESPONSE_JSON_FMT,
    & update_preconditions.current_version);
    int update_version = 12235;
    if (update_preconditions.current_version < update_version) {
        tcp_send_file(FIRMWARE_FILE_PATH);
    } else {
        log_error("Satellite is up to date");
        return 1;
    }
    return 0;
}

int earth_surface_scan(){
    
    rpc earth_scan_request = {
        SCAN_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        NULL};
    tcp_send_rpc_request(& earth_scan_request);
    rpc response;
    scan_response scan_preconditions;
    tcp_recv_rpc_response(& response);
    int result = json_scanf(response.payload, strlen(response.payload),
    SCAN_RESPONSE_JSON_FMT,
    & scan_preconditions.slices_quantity,
    & scan_preconditions.slice_size_bytes);
    log_trace("slices:%i,slice_size:%i",scan_preconditions.slices_quantity,
    scan_preconditions.slice_size_bytes);
    for(int i=0; i<scan_preconditions.slices_quantity;i++){
        char filename[200];
        sprintf(filename,"slice_%i.jpg",i);
        log_trace("%s",filename);
        FILE* file_ptr = fopen(filename,"w");
        tcp_recv_file(file_ptr);
    }
}

int get_telemetry_data(sat_telemetry* telemetry_data){
    rpc telemetry_request = {
        TELEMETY_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        NULL};
    rpc telemetry_response;
    tcp_send_rpc_request(&telemetry_request);
    tcp_recv_rpc_response(&telemetry_response);
    
    int result = json_scanf(telemetry_response.payload, strlen(telemetry_response.payload),
    TELEMETRY_JSON_FMT,
    & telemetry_data->cpu_usage,
    & telemetry_data->firm_version,
    & telemetry_data->mem_usage,
    & telemetry_data->satellite_id,
    & telemetry_data->uptime);

    log_trace("cpu_usage:%f,firmware_version:%i,mem_usage:%f,satellite_id:%i,uptime:%f",
    telemetry_data->cpu_usage,
    telemetry_data->firm_version,
    telemetry_data->mem_usage,
    telemetry_data->satellite_id,
    telemetry_data->uptime
    );
    return 0;
}

int stop_sat_service(){
    tcp_close_connection();
    return 0;
}