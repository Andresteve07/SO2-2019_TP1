#include <stdio.h>
#include "satellite_service.h"
#include "shared/socket_operation.h"
#include "frozen.h"
#include "log.h"
#include "string.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <errno.h>


#define UPDATE_PARAMS_FMT "{update_version:%d, file_size_bytes:%lu}"
#define UPDATE_RESULT_FMT "{current_version:%d}"
#define TELEMETRY_RESULT_FMT "{cpu_usage:%f, firmware_version:%d, mem_usage:%f, satellite_id:%d, uptime:%f}"
#define SCAN_RESULT_JSON_FMT "{slice_quantity:%d, slices_dataset:%M}"
#define RPC_ERROR_FMT "{error_code:%d,error_message:%Q}"

#define SCAN_META_FMT "{file_name:%Q,file_size_bytes:%lu}"


#define UPDATE_COMMAND_CODE 1

#define TELEMETY_COMMAND_CODE 3

#define SCAN_COMMAND_CODE 2
#define FIRMWARE_COMMAND_CODE 1

#define STATION_ID 123456
#define SATELLITE_ID 987654

#define FIRMWARE_FILE_PATH "../assets/so2_tp1-sat"
#define FIRMWARE_DIR_PATH "../assets/firmware_upgrade/"

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

long find_file_size(char file_rel_path[]) 
{ 
    // opening the file in read mode 
    FILE* fp = fopen(file_rel_path, "r"); 
    // checking if the file exist or not 
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
    fseek(fp, 0L, SEEK_END); 
    // calculating the size of the file 
    long res = ftell(fp); 
    // closing the file 
    fclose(fp);   
    return res; 
}

int myAtoi(char* str) 
{ 
    int res = 0; // Initialize result 
  
    // Iterate through all characters of input string and 
    // update result 
    for (int i = 0; str[i] != '\0'; ++i){
        log_trace("DIGIT: %c",str[i]);
        if(str[i] >= '0' && str[i] <= '9'){
            res = res * 10 + str[i] - '0';
        }
    }
    // return result. 
    return res; 
} 

int fill_upgrade_params(update_params* params){
    DIR * dirp;
    struct dirent * entry;
    long file_size = 0;

    dirp = opendir(FIRMWARE_DIR_PATH); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            char rel_path[256];
            bzero(rel_path,sizeof(rel_path));
            sprintf(rel_path, "%s%s",FIRMWARE_DIR_PATH,entry->d_name);
            int path_length = strlen(rel_path);
            params->file_path=malloc(path_length + 1);
            bzero(params->file_path,path_length+1);
            strcpy(params->file_path, rel_path);
            file_size = find_file_size(rel_path);
            log_trace("UPGRADE FILE PATH:%s",params->file_path);
            log_trace("UPGRADE FILE. name:%s,size%lu",entry->d_name,file_size);
            if (file_size > 500) {
                char* token = strtok(entry->d_name,"__");
                char* version_str; 
                while (token!=NULL){
                    token = strtok(NULL,"__");
                    if (token!=NULL){
                        version_str = token;
                    }
                }
                log_trace("VERSIONSTR: %s",version_str);
                int version_num = myAtoi(version_str);
                params->file_size_bytes = file_size;
                params->update_version= version_num;
                log_trace("UPDATE PARAMS. version:%i,size%lu",params->update_version,params->file_size_bytes);
                closedir(dirp);
                return 0;
            }
        }
    }
    closedir(dirp);
}

int upgrade_firmware(char* file_name){
    update_params req_params;
    //req_params.update_version = get_update_version();
    //req_params.file_size_bytes = get_update_file_size();
    fill_upgrade_params(& req_params);

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
    log_trace("FIRM_UPDATE: %s",req_params.file_path);
    tcp_send_file(req_params.file_path);
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
    struct timeval start, end;
    double cpu_time_used;
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
        scan_metadata metadata;
        json_scanf(response.payload,
        strlen(response.payload),
        SCAN_META_FMT,
        & metadata.file_name,
        & metadata.file_size_bytes);
        
        log_trace("scan_name: %s, size: %lu", 
        metadata.file_name,
        metadata.file_size_bytes);
        char filename[200];
        bzero(filename,200);
        sprintf(filename,"../assets/scans/%s",metadata.file_name);
        log_trace("FILENAME: %s",filename);
        FILE* file_ptr = fopen(filename,"wb");
        gettimeofday(&start,NULL);
        if(tcp_recv_file_known_size(file_ptr,metadata.file_size_bytes)==socket_success){
            gettimeofday(&end,NULL);
            //cpu_time_used = ((double) (end - start)) / (double) CLOCKS_PER_SEC;
            log_trace("File transfer succeed after: %lu microseconds",(end.tv_sec-start.tv_sec)*1000000L + end.tv_usec - start.tv_usec);
            log_debug("File: %s SUCCESSFULLY RECEIVED!", filename);
        } else {
            gettimeofday(&end,NULL);
            //cpu_time_used = ((double) (end - start)) / (double) CLOCKS_PER_SEC;
            log_trace("File transfer failed after: %d microseconds",(end.tv_sec-start.tv_sec)*1000000L + end.tv_usec - start.tv_usec);
            log_error("File: %s COULD NOT BE RECEIVED!", filename);
            return 1;
        }    
    } else {
        return process_rpc_error(response.error);
    }
    return 0;

}
int old_earth_surface_scan(){
    
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