//
// Created by sps5394 on 11/7/18.
//

#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H
#define MAX_FILENAME 128
#include <cmpsc311_network.h>
#include <cmpsc311_log.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "proto.h"
////////////////////////////////////////////////////////////////////////////////
//
// Function     : server_create
// Description  : Creates the server on the port listed
//
// Inputs       : port: Port of the server for TCP connections
// Outputs      : socketfh if successful, -1 if failure
int server_create(int port);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : server_run
// Description  : Starts the server and listens for any incoming connections
// Inputs       : socketfh: File handle of connected socket
// Outputs      : 0 if successful, -1 if failure
int server_run(int socketfh);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : server_func
// Description  : Thread function to respond to a new client
// Inputs       : arg: File handle of connected socket
// Outputs      : 0 if successful, -1 if failure
void *server_func(void *arg);

void *exit_connection(int sockfd, ProtoMsg *msg, const unsigned char *marshallBuffer);
          // Cleans up connection

#endif //CLIENT_SERVER_H
