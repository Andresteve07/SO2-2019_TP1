#include <stdio.h>
#include "satellite_service.h"
#include "shared/socket_operation.h"
#include "frozen.h"
#include "log.h"
#include "string.h"
#include <unistd.h>
#include <stdlib.h>

#define UPDATE_PARAMS_FMT "{update_version:%d, file_size_bytes:%lu}"
#define UPDATE_RESULT_FMT "{current_version:%d}"
#define TELEMETRY_RESULT_FMT "{cpu_usage:%f, firmware_version:%d, mem_usage:%f, satellite_id:%d, uptime:%f}"
#define SCAN_RESULT_JSON_FMT "{slice_quantity:%d, slices_dataset:%M}"
#define RPC_ERROR_FMT "{error_code:%d,error_message:%Q}"


#define UPDATE_COMMAND_CODE 1

#define TELEMETY_COMMAND_CODE 3

#define SCAN_COMMAND_CODE 2
#define FIRMWARE_COMMAND_CODE 1

#define STATION_ID 123456
#define SATELLITE_ID 987654

#define FIRMWARE_FILE_PATH "../assets/so2_tp1-sat"

int init_sat_service(char* server_address){
    tcp_init_client();
    tcp_connect_to_server(server_address);
    udp_init_client(server_address);
    return 0;
}

int process_rpc_error(char* error_data){
    rpc_error response_error;
    if (error_data!=NULL){
            int result = json_scanf(error_data,
            strlen(error_data),
            RPC_ERROR_FMT,
            & response_error.error_code,
            & response_error.error_message);
            log_error("ERROR - CODE:%i MESG: %s",response_error.error_code,response_error.error_message);
            return 1;
    } else {
        return 1;
    }
}

int get_update_version(){
    //TODO get version from file name using regex.
    return 10512;
}

long get_update_file_size(){
    //TODO get size from upgrade file.
    return 1024;
}

int upgrade_firmware(char* file_name){
    update_params req_params;
    req_params.update_version = get_update_version();
    req_params.file_size_bytes = get_update_file_size();

    char rpc_buf[RPC_MSG_BUF_SIZE-4];
	struct json_out output = JSON_OUT_BUF(rpc_buf, sizeof(rpc_buf));
	json_printf(&output, UPDATE_PARAMS_FMT, req_params.update_version,req_params.file_size_bytes);

    rpc update_request = {
        UPDATE_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        rpc_buf,
        NULL};
    tcp_send_rpc(& update_request);
    rpc firmware_update_response;
    tcp_recv_rpc(& firmware_update_response);
    update_result result;
    if (firmware_update_response.payload!=NULL){
        int fields_loaded = json_scanf(firmware_update_response.payload, 
        strlen(firmware_update_response.payload),
        UPDATE_RESULT_FMT,
        & result.current_version);
        log_debug("UPDATE RESP - CURRENT_VERSION: %i", result.current_version);
    } else {
        return process_rpc_error(firmware_update_response.payload);
    }

    tcp_send_file(FIRMWARE_FILE_PATH);
    return 0;
}

slice_meta slices_dataset_array[500];
void scan_slices_dataset(const char* str_json, slice_meta* result_data_set){
    char* slice_name;
    long slice_size;
	long length = strlen(str_json);

    struct json_token token;
    for (int i = 0; json_scanf_array_elem(str_json, length, ".slices_dataset",i,& token) > 0; i++){
        slice_meta slice_data;
		log_trace("TOKEN: %c, %lu", *token.ptr,token.len);
        json_scanf(token.ptr,token.len,SLICE_META_FMT, & slice_name,& slice_size);
		log_trace("PARSED NAME: %s SIZE: %lu",slice_name,slice_size);
        long bytes_for_name = strlen(slice_name) + 1;
        slice_data.slice_name=malloc(bytes_for_name);
        bzero(slice_data.slice_name,bytes_for_name);
        strcpy(slice_data.slice_name,slice_name);
        slice_data.slice_size_bytes = slice_size;
        slices_dataset_array[i] = slice_data;
    }

    result_data_set = slices_dataset_array;
}

int earth_surface_scan(){
    
    rpc earth_scan_request = {
        SCAN_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        NULL,
        NULL};
    tcp_send_rpc(& earth_scan_request);
    rpc response;
    scan_result result;
    tcp_recv_rpc(& response);
    if (response.payload!=NULL){
        int fields_loaded = json_scanf(response.payload, strlen(response.payload),
        "{slice_quantity:%d}",
        & result.slices_quantity);
        scan_slices_dataset(response.payload,result.slices_dataset);
        log_trace("slice_quantity:%i",result.slices_quantity);
        //TODO Why using result.slices_dataset throws segmentation fault????
        for (int i = 0; i < result.slices_quantity; i++){
            log_trace("order:%i, slice_name:%s,slice_size:%lu",i,
            slices_dataset_array[i].slice_name,
            slices_dataset_array[i].slice_size_bytes);
            char filename[200];
            bzero(filename,200);
            sprintf(filename,"../assets/scans/%s",slices_dataset_array[i].slice_name);
            log_trace("FILENAME: %s",filename);
            FILE* file_ptr = fopen(filename,"wb");
            if (file_ptr != NULL){
                if(tcp_recv_file_known_size(file_ptr,slices_dataset_array[i].slice_size_bytes)==socket_success){
                    log_debug("File: %s SUCCESSFULLY RECEIVED!", filename);
                } else {
                    log_error("File: %s COULD NOT BE RECEIVED!", filename);
                    return 1;
                }    
            } else{
                log_error("Failed to create file: %s",filename);
                return 1;
            }
        }    
    } else {
        return process_rpc_error(response.error);
    }
    return 0;
}

int get_telemetry_data(sat_telemetry* telemetry_data){
    rpc telemetry_request = {
        TELEMETY_COMMAND_CODE,
        STATION_ID,
        SATELLITE_ID,
        NULL,
        NULL};
    rpc telemetry_response;
    udp_send_rpc(&telemetry_request);
    udp_recv_rpc(&telemetry_response);
    if (telemetry_response.payload!=NULL){
        int result = json_scanf(telemetry_response.payload, strlen(telemetry_response.payload),
        TELEMETRY_RESULT_FMT,
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
    } else {
        return process_rpc_error(telemetry_response.error);
    }

    return 0;
}

int stop_sat_service(){
    tcp_close_connection();
    return 0;
}