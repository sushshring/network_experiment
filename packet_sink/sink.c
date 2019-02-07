//
// Created by sps5394 on 11/7/18.
//
#include <fcntl.h>
#include "sink.h"
struct timespec tstart = {0,0};

int sink_create(int port) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int serv_sock;
  if ((serv_sock = cmpsc311_connect_server_udp((unsigned short) port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return -1;
  }
  return serv_sock;
}

int sink_run(int socketfh) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  char *rbuf = malloc(MAX_BLOCK_SIZE);
  while (1) {
    if (read(socketfh, rbuf, MAX_BLOCK_SIZE) == -1) {
      logMessage(LOG_ERROR_LEVEL, "Failed to read from socket\n");
    }
  }
  free(rbuf);
}