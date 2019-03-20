//
// Created by sps5394 on 11/5/18.
//

#include "client.h"

struct timespec tstart = {0,0};
struct timespec tend = {0, 0};

int client_connect(unsigned char *server_addr, uint16_t port) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int socketfh;

  if (( socketfh = cmpsc311_client_connect(server_addr, port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Failed to connect to server\n");
    return ( -1 );
  }

  // Setup timing log
  if (( timing_logfh = open(TIME_LOG_NAME, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) ==
      -1) {
    // Error out
    logMessage(LOG_ERROR_LEVEL, "Error opening log : %s (%s)", TIME_LOG_NAME, strerror(errno));
    return ( -1 );
  }

  return socketfh;
}

void *flooder_checks(void *flooder_fh) {
  unsigned char *msg = malloc(6);
  while (1) {
    cmpsc311_read_bytes(flooder_fh, 6, msg);
    if (strncmp(msg, "START", 6) == 0) {
      write(timing_logfh, "FLOODER_START", 14);
    } else if (strncmp(msg, "ENDIN", 6)) {
      write(timing_logfh, "FLOODER_END", 14);
    }
  }
}

int client_run(int socketfh, int flooder_fh) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  ProtoMsg msg;
  unsigned char *marshallBuffer = malloc(sizeof(ProtoMsg));
  int fileavlb = 0, readBytes = 0;
  pthread_t newthread;
  int *flooder_sock = malloc(sizeof(int));
  *flooder_sock = flooder_fh;
  pthread_create(&newthread, NULL, flooder_checks, flooder_sock);
  pthread_detach(newthread);
  logMessage(LOG_INFO_LEVEL, "Requesting file\n");
  // Request file
  bzero(&msg, sizeof(ProtoMsg));
  msg.msgType = REQ_FILE;
  memcpy(msg.data, FILENAME, strlen(FILENAME));
  marshall(&msg, marshallBuffer);
  cmpsc311_send_bytes(socketfh, sizeof(ProtoMsg), marshallBuffer);

  // Wait for ack
  logMessage(LOG_INFO_LEVEL, "Waiting for ack\n");
  bzero(&msg, sizeof(ProtoMsg));
  cmpsc311_read_bytes(socketfh, sizeof(ProtoMsg), marshallBuffer);
  unmarshall(marshallBuffer, &msg);
  CMPSC_ASSERT2(msg.msgType == REQ_ACK, "Expecting message type %d, got %d\n", REQ_ACK,
                msg.msgType);

  fileavlb = 1;
  // Read file data
  while (fileavlb) {
    // Wait for block
    log_request_start();
    logMessage(LOG_INFO_LEVEL, "Waiting for block. Currently read %d bytes\n", readBytes);
    bzero(&msg, sizeof(ProtoMsg));
    cmpsc311_read_bytes(socketfh, sizeof(ProtoMsg), marshallBuffer);
    unmarshall(marshallBuffer, &msg);
    logAssert(msg.msgType == FILE_BLOCK || msg.msgType == FILE_COMPLETE, __FILE__, __LINE__,
                  "Expecting message type %d or %d, got %d\n", FILE_BLOCK, FILE_COMPLETE,
                  msg.msgType);
    readBytes += MAX_BLOCK_SIZE;
    switch (msg.msgType) {
      case FILE_BLOCK:
        // continue reading
        logMessage(LOG_INFO_LEVEL, "Received block\n");
        bzero(&msg, sizeof(ProtoMsg));
        msg.msgType = FILE_BLOCK_ACK;
        marshall(&msg, marshallBuffer);
        cmpsc311_send_bytes(socketfh, sizeof(ProtoMsg), marshallBuffer);
        log_request_end();
        break;
      case FILE_COMPLETE:
        // continue reading
        logMessage(LOG_INFO_LEVEL, "File complete. Exiting\n");
        bzero(&msg, sizeof(ProtoMsg));
        msg.msgType = FILE_COMPLETE_ACK;
        marshall(&msg, marshallBuffer);
        cmpsc311_send_bytes(socketfh, sizeof(ProtoMsg), marshallBuffer);
        log_request_end();
        fileavlb = 0;
        break;
      default:
        logMessage(LOG_ERROR_LEVEL, "Illegal request type\n");
        free(marshallBuffer);
        return ( -1 );
    }
  }
  close(socketfh);
  free(marshallBuffer);
  return 0;
}

void log_request_start() {
  clock_gettime(CLOCK_REALTIME, &tstart);
  char msg[128];
  sprintf(msg, "START: %ld\n", BILLION * tstart.tv_sec + tstart.tv_nsec);
  write(timing_logfh, msg, strlen(msg));
}

void log_request_end() {
  clock_gettime(CLOCK_REALTIME, &tend);
  char msg[128];
  sprintf(msg, "END: %ld\n", BILLION * tend.tv_sec + tend.tv_nsec);
  write(timing_logfh, msg, strlen(msg));
}
