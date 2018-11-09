//
// Created by sps5394 on 11/7/18.
//

#ifndef CLIENT_FLOODER_H
#define CLIENT_FLOODER_H

#include <cmpsc311_network.h>
#include <cmpsc311_log.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "proto.h"
////////////////////////////////////////////////////////////////////////////////
//
// Function     : flooder_create
// Description  : Creates the flooder on the sink address
//
// Inputs       : port: Port of the flooder for TCP connections
//                flooder_type: Type of the flooder. 1: self loop. 2: packet-sink
// Outputs      : socketfh if successful, -1 if failure
int flooder_create(char *addr, int port, int flooder_type);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : flooder_run
// Description  : Starts the flooder
// Inputs       : socketfh: File handle of connected socket
// Outputs      : 0 if successful, -1 if failure
int flooder_run(int socketfh);


#endif //CLIENT_flooder_H
