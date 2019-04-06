/*
 * socket_client.h
 *
 *  Created on: 1 abr. 2019
 *      Author: steve-urbit
 * 
 * This header file provides high level abstractions for socket operations.
 * wW sugest mock socket server interactions using netcat
 * $ netcat -l -p 12121
 * For the socket client
 * $ netcat localhost 12121
 */

#ifndef SRC_SOCKET_CLIENT_H_
#define SRC_SOCKET_CLIENT_H_

#include<stdio.h> 

typedef enum{
	socket_success,
	socket_failure,
	socket_unknown
}operation_result;

typedef enum{
	TCP,
	UDP,
	OTHER
}connection_type;
/*
typedef struct rpc_request
{
	unsigned char command_id;
	int satellite_id;
	int station_id;
	size_t payload_size;
} rpc_request;
*/
typedef struct rpc
{
	unsigned char command_id;
	int satellite_id;
	int station_id;
	char* payload;
} rpc;

operation_result tcp_init();
operation_result tcp_timeouts(int seconds);
operation_result tcp_connect_to_server(char*  server_ip);
operation_result tcp_send_data(char* data_buffer);
operation_result tcp_send_data_bytes(char* data_buffer, size_t byte_count);
operation_result tcp_recv_data(char* data_buffer);
operation_result tcp_recv_data_bytes(char* data_buffer, size_t byte_count);
operation_result tcp_send_rpc_request(rpc* request);
operation_result tcp_recv_rpc_response(rpc* response);
operation_result tcp_send_file(char* file_name);
operation_result tcp_recv_file(FILE* input_file);
operation_result tcp_recv_file_known_size(FILE* input_file, size_t byte_count);
operation_result tcp_close_connection();

operation_result udp_connect();
operation_result udp_send_data();
operation_result udp_recv_data();


#endif /* SRC_SOCKET_CLIENT_H_ */