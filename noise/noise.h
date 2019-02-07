//
// Created by Sushrut Shringarputale on 2019-01-22.
//

#ifndef NOISE_NOISE_H
#define NOISE_NOISE_H

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <cmpsc311_log.h>
#include <cmpsc311_network.h>

int noise_amt = 1;

////////////////////////////////////////////////////////////////////////////////
//
// Function     : noise_connect
// Description  : Connects the noise generator to the server as specified
//
// Inputs       : server_addr: IP address of the server
//                port: Port of the server for TCP connection
// Outputs      : socketfh if successful, -1 if failure
int noise_connect(unsigned char *server_addr, uint16_t port);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : noise_run
// Description  : The simulator for the noise generator. Constantly listens
//                 on stdin for noise rate and generates a constant amount of
//                  noise
// Inputs       : socketfh: File handle of connected socket
// Outputs      : 0 if successful test, -1 if failure
int noise_run(int socketfh);

int get_noise_bytes(int input_parameter);

void * sysin_listener(void *);

#endif //NOISE_NOISE_H
