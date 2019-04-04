/*
 * tcp_socket_client.c
 *
 *  Created on: 1 abr. 2019
 *      Author: steve-urbit
 */
#include "socket_client.h"
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "frozen.h"
 
#define PORT 12121 
#define SA struct sockaddr 
#define RPC_JSON_FMT "{command_id:%d,satellite_id:%d,station_id:%d,payload_size:%d,payload:%Q}"

int sockfd, connfd;
struct sockaddr_in servaddr, cli;
char* response_buffer;

operation_result tcp_init(){
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        return socket_failure; 
    } 
    else {
		printf("Socket successfully created..\n"); 
		return socket_success;
	}
}

operation_result tcp_connect_to_server(char*  server_ip){
	bzero(&servaddr, sizeof(servaddr)); 
	// assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        return socket_failure; 
    } else {
		printf("connected to the server..\n");
		return socket_success;
	}
}

operation_result tcp_send_data(char* data_buffer){
	if(write(sockfd, data_buffer, sizeof(data_buffer)) > 0){
		return socket_success;
	} else {
		return socket_failure;
	}
}

operation_result tcp_send_data_bytes(char* data_buffer, size_t byte_count){
	if(write(sockfd, data_buffer, byte_count) > 0){
		return socket_success;
	} else {
		return socket_failure;
	}
}

operation_result tcp_recv_data_bytes(char* data_buffer, size_t byte_count){
	if(read(sockfd, data_buffer, byte_count) > 0){
	return socket_success;
	} else {
		return socket_failure;
	}
}

operation_result tcp_recv_data(char* data_buffer){
	if(read(sockfd, data_buffer, sizeof(data_buffer)) > 0){
	return socket_success;
	} else {
		return socket_failure;
	}
}

void load_heading_integer_to_byte_array(int number,char* array){
	//LSB first
	printf("number: %i ", number);
	array[0] = (unsigned char) (number & 0xFF);
	array[1] = (unsigned char) ((number>>8) & 0xFF);
	array[2] = (unsigned char) ((number>>16) & 0xFF);
	array[3] = (unsigned char) ((number>>24) & 0xFF);
	printf("bytes:%i,%i,%i,%i\n",array[0],array[1],array[2],array[3]);

}

operation_result tcp_send_rpc_request(rpc* request){
	char total_buf[204];
	bzero(total_buf,sizeof(total_buf));

	char* rpc_buf = &total_buf[4];
	struct json_out output = JSON_OUT_BUF(rpc_buf, 200);

	json_printf(&output, RPC_JSON_FMT,
	request->command_id,
	request->satellite_id,
	request->station_id,
	request->payload_size,
	request->payload);
	printf("RPC req: %s\n",rpc_buf);

	load_heading_integer_to_byte_array(strlen(rpc_buf),total_buf);
	
	printf("TOTAL req: %c%c%c%c%s\n",total_buf[0],total_buf[1],total_buf[2],total_buf[3],&total_buf[4]);

	if(write(sockfd, total_buf, strlen(rpc_buf)+4) > 0){
		return socket_success;
	} else {
		return socket_failure;
	}
	
}
int integerFromArrayTip(char* array){
	int number = 0;
	number = (int)((unsigned char)array[3]);
	number = (number<<8) + (unsigned char) array[2];
	number = (number<<8) + (unsigned char) array[1];
	number = (number<<8) + (unsigned char) array[0];
	printf("number:%i",number);
	return number;

}

operation_result tcp_recv_rpc_response(rpc* response){
	size_t payload_size;
	char resp_buf[500];//sizeof only works ok for static arrays i.e. results on 500
	bzero(resp_buf, sizeof(resp_buf));
	if(read(sockfd, resp_buf, sizeof(resp_buf)) > 0){
		int size_int = integerFromArrayTip(resp_buf);
		char* recv_data = &resp_buf[4];
		recv_data[size_int]='\0';
		printf("size_cadena:%lu,cadena: %s\n",strlen(recv_data),recv_data);
		printf("HOLOOO");
		
		json_scanf(recv_data,strlen(recv_data),RPC_JSON_FMT,
		&response->command_id,
		&response->satellite_id,
		&response->station_id,
		&response->payload_size,
		&response->payload);
		
		printf("cid:%i,\nsatid:%i,\nstid:%i,\nsize:%i,\npay:%lu",
		response->command_id,
		response->satellite_id,
		response->station_id,
		response->payload_size,
		strlen(response->payload));
		
		return socket_success;

	} else {
		return socket_failure;
	}
}


operation_result tcp_send_file(char* file_name){}
operation_result tcp_recv_file(){}

operation_result tcp_close_connection(){
	// close the socket 
    close(sockfd);
	return socket_success;
}