//
// Created by sps5394 on 11/7/18.
//
#include <fcntl.h>
#include "flooder.h"

void *exit_connection(int sockfd, ProtoMsg *msg, const unsigned char *marshallBuffer);

int server_create(int port) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int serv_sock;
  if ((serv_sock = cmpsc311_connect_server((unsigned short) port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return -1;
  }
  return serv_sock;
}

int server_run(int socketfh) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  while (1) {
    logMessage(LOG_INFO_LEVEL, "Accepting new connections\n");
    int *client_sock = malloc(sizeof(int));
    if (( *client_sock = cmpsc311_accept_connection(socketfh)) == -1) {
      logMessage(LOG_ERROR_LEVEL, "Failed to accept connection\n");
      free(client_sock);
      return -1;
    }
    logMessage(LOG_INFO_LEVEL, "Accepted new connection at fd: %d\n", *client_sock);
    pthread_t newthread;
    pthread_create(&newthread, NULL, server_func, client_sock);
    pthread_detach(newthread);
  }
}


void *server_func(void *arg) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int sockfd = *(int *)arg;
  ProtoMsg msg;
  unsigned char *marshallBuffer = malloc(sizeof(ProtoMsg));
  char filename[MAX_FILENAME];
  int fd;
  FILE *readfile;

  free(arg);
  arg = NULL;
  logMessage(LOG_INFO_LEVEL, "Starting to server file request\n");
  // Get file request
  bzero(&msg, sizeof(ProtoMsg));
  cmpsc311_read_bytes(sockfd, sizeof(ProtoMsg), marshallBuffer);
  unmarshall(marshallBuffer, &msg);
  CMPSC_ASSERT2(msg.msgType == REQ_FILE, "Expecting message type %d, got %d\n", REQ_FILE,
                msg.msgType);
  memcpy(filename, msg.data, MAX_FILENAME);
  logMessage(LOG_INFO_LEVEL, "Got request for file: %s\n", filename);

  bzero(&msg, sizeof(ProtoMsg));
  msg.msgType = REQ_ACK;
  marshall(&msg, marshallBuffer);
  cmpsc311_send_bytes(sockfd, sizeof(ProtoMsg), marshallBuffer);

  // Read file and send to client
  if( access( filename, R_OK ) == -1 ) {
    // FILE DOES NOT EXIST
    return exit_connection(sockfd, &msg, marshallBuffer);
  }
  logMessage(LOG_INFO_LEVEL, "Requested file exists\n");
  if ((fd = open(filename, O_RDONLY, S_IRUSR)) == -1) {
    return exit_connection(sockfd, &msg, marshallBuffer);
  }
  readfile = fdopen(fd, "r");
  while (!feof(readfile)) {
    if (fread(msg.data, 1, MAX_BLOCK_SIZE, readfile) <= 0) {
      logMessage(LOG_ERROR_LEVEL, "Could not read from file\n");
      break;
    }
    logMessage(LOG_INFO_LEVEL, "Sending file block\n");
    bzero(&msg, sizeof(ProtoMsg));
    msg.msgType = FILE_BLOCK;
    marshall(&msg, marshallBuffer);
    cmpsc311_send_bytes(sockfd, sizeof(ProtoMsg), marshallBuffer);

    bzero(&msg, sizeof(ProtoMsg));
    cmpsc311_read_bytes(sockfd, sizeof(ProtoMsg), marshallBuffer);
    unmarshall(marshallBuffer, &msg);
    CMPSC_ASSERT2(msg.msgType == FILE_BLOCK_ACK, "Expecting message type %d, got %d\n", FILE_BLOCK_ACK,
                  msg.msgType);
    logMessage(LOG_INFO_LEVEL, "File block acked\n");
  }
  return exit_connection(sockfd, &msg, marshallBuffer);
}

void *exit_connection(int sockfd, ProtoMsg *msg, const unsigned char *marshallBuffer) {
  bzero(msg, sizeof(ProtoMsg));
  msg->msgType = FILE_COMPLETE;
  marshall(msg, (unsigned char *) marshallBuffer);
  while (!cmpsc311_send_bytes(sockfd, sizeof(ProtoMsg), (unsigned char *) marshallBuffer));
  close(sockfd);
  return NULL;
}
