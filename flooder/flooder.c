//
// Created by sps5394 on 11/7/18.
//
#include <fcntl.h>
#include "flooder.h"

int flooder_create(char *addr, int port, int flooder_type) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int serv_sock;
  if (flooder_type == 1) addr = "127.0.0.1";
  if ((serv_sock = cmpsc311_client_connect_udp((unsigned char *) addr, (unsigned short) port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return -1;
  }
  return serv_sock;
}

int flooder_run(int socketfh) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  char *rbuf = malloc(MAX_BLOCK_SIZE);
  int rfh;
  time_t clocktime, currenttime;
  struct timespec clock = {0, 0};

  if ((rfh = open("/dev/urandom", O_RDONLY, S_IRUSR)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not open /dev/urandom\n");
    return -1;
  }
  while (1) {
    sleep(5);
    clock_gettime(CLOCK_MONOTONIC, &clock);
    clocktime = clock.tv_sec;
    currenttime = clocktime;
    logMessage(LOG_INFO_LEVEL, "Running flooder\n");
    while (currenttime < clocktime + 5) {
      if (read(rfh, rbuf, MAX_BLOCK_SIZE) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to read from /dev/urandom\n");
        continue;
      }
      if (write(socketfh, rbuf, MAX_BLOCK_SIZE) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to write to UDP socket\n");
      }
      clock_gettime(CLOCK_MONOTONIC, &clock);
      currenttime = clock.tv_sec;
    }
  }
  free(rbuf);
}
