//
// Created by sps5394 on 11/5/18.
//

#ifndef NETWORK_EXPERIMENT_CLIENT_H
#define NETWORK_EXPERIMENT_CLIENT_H
#define FILENAME "large_file.txt"
#define TIME_LOG_NAME "client_times"
#define LAMBDA 0.1
#define BILLION 1000000000L

#ifndef __linux__
// CLION IMPORTS
#include <strings.h>
#include <zconf.h>
#endif
// END CLION IMPORTS
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cmpsc311_network.h>
#include <cmpsc311_log.h>
#include <pthread.h>
#include "../proto/proto.h"
#include "../common/ccommon.h"

// GLOBAL VARIABLES
int timing_logfh;
int flooder_state;
pthread_mutex_t lock;
extern struct timespec tstart;
extern struct timespec tend;

////////////////////////////////////////////////////////////////////////////////
//
// Function     : client_connect
// Description  : Connects the client to the server as specified
//
// Inputs       : server_addr: IP address of the server
//                port: Port of the server for TCP connection
// Outputs      : socketfh if successful, -1 if failure
int client_connect(unsigned char *server_addr, uint16_t port);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : client_run
// Description  : The simulator for the client. Requests a specific file at a
//                exponentially distributed time
// Inputs       : socketfh: File handle of connected socket
// Outputs      : 0 if successful test, -1 if failure
int client_run(int socketfh);

int client_send_request(int socketfh, int *request_counter);

void log_request_start();

void log_request_end(int *request_counter);

#endif //NETWORK_EXPERIMENT_CLIENT_H
