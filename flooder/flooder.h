//
// Created by sps5394 on 11/7/18.
//

#ifndef CLIENT_FLOODER_H
#define CLIENT_FLOODER_H
#define TIME_LOG_NAME "flooder_times"
#define BILLION 1000000000L
#define BLOCK_SIZE 3400
#include <cmpsc311_network.h>
#include <cmpsc311_log.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "proto.h"

typedef struct {
  int udp_sock;
  int client_sock;
  int scale;
} flooder_socks;
// GLOBAL VARIABLES
int timing_logfh;
extern struct timespec tstart;
////////////////////////////////////////////////////////////////////////////////
//
// Function     : flooder_create
// Description  : Creates the flooder on the sink address
//
// Inputs       : port: Port of the flooder for TCP connections
//                flooder_type: Type of the flooder. 1: self loop. 2: packet-sink
// Outputs      : socketfh if successful, -1 if failure
flooder_socks *flooder_create(char *addr, int port, char *client_addr, int client_port, int scale);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : flooder_run
// Description  : Starts the flooder
// Inputs       : socketfh: File handle of connected socket
// Outputs      : 0 if successful, -1 if failure
int flooder_run(flooder_socks *);

void log_request_start();
#endif //CLIENT_flooder_H
