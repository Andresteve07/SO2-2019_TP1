#include <stdio.h>
#include "satellite_service.h"
#include "socket_client.h"
#include "frozen.h"

#define TELEMETRY_JSON_FMT "{cpu_usage:%f,firmware_version:%d,mem_usage:%f,satellite_id:%d,uptime:%f}"
#define TELEMETY_COMMAND_CODE 3
#define SCAN_COMMAND_CODE 2
#define FIRMWARE_COMMAND_CODE 1

#define STATION_ID 123456
#define SATELLITE_ID 987654

int init_sat_service(){
    tcp_init();
    tcp_connect_to_server("sad");
}

int get_telemetry_data(sat_telemetry* telemetry_data){
    rpc telemetry_request = {
        TELEMETY_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        0,NULL};
    rpc telemetry_response;
    tcp_send_rpc_request(&telemetry_request);
    tcp_recv_rpc_response(&telemetry_response);
    json_scanf(telemetry_response.payload,telemetry_response.payload_size,
    TELEMETRY_JSON_FMT,
    telemetry_data->cpu_usage,
    telemetry_data->firm_version,
    telemetry_data->mem_usage,
    telemetry_data->satellite_id,
    telemetry_data->uptime);
    
    return 0;
}

int stop_sat_service(){
    tcp_close_connection();
    return 0;
}