//
// Created by sps5394 on 11/7/18.
//

#ifndef CLIENT_FLOODER_H
#define CLIENT_FLOODER_H
#define TIME_LOG_NAME "flooder_times"
#define BILLION 1000000000L
#include <cmpsc311_network.h>
#include <cmpsc311_log.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "proto.h"

// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////////////
//
// Function     : flooder_create
// Description  : Creates the flooder on the sink address
//
// Inputs       : port: Port of the flooder for TCP connections
//                flooder_type: Type of the flooder. 1: self loop. 2: packet-sink
// Outputs      : socketfh if successful, -1 if failure
int sink_create(int port);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : flooder_run
// Description  : Starts the flooder
// Inputs       : socketfh: File handle of connected socket
// Outputs      : 0 if successful, -1 if failure
int sink_run(int socketfh);

void log_request_start();
#endif //CLIENT_flooder_H
