//
// Created by sps5394 on 11/7/18.
//
#include <fcntl.h>
#include "flooder.h"
struct timespec tstart = {0,0};

int flooder_create(char *addr, int port, int flooder_type) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int serv_sock;
  if (flooder_type == 1) addr = "127.0.0.1";
  logMessage(LOG_INFO_LEVEL, "Writing to UDP socket: %s:%d", addr, port);
  if ((serv_sock = cmpsc311_client_connect_udp((unsigned char *) addr, (unsigned short) port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return -1;
  }
  // Setup timing log
  if (( timing_logfh = open(TIME_LOG_NAME, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) ==
      -1) {
    // Error out
    logMessage(LOG_ERROR_LEVEL, "Error opening log : %s (%s)", TIME_LOG_NAME, strerror(errno));
    return ( -1 );
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
  long send_bytes = 0, sent_bytes = 0;

  if ((rfh = open("/dev/urandom", O_RDONLY, S_IRUSR)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not open /dev/urandom\n");
    return -1;
  }
  while (1) {
    sleep(5);
    clock_gettime(CLOCK_REALTIME, &clock);
    clocktime = clock.tv_sec;
    currenttime = clocktime;
    logMessage(LOG_INFO_LEVEL, "Running flooder\n");
    log_request_start();
    send_bytes = 0;
    sent_bytes = 0;
    while (currenttime < clocktime + 5) {
      if (read(rfh, rbuf, MAX_BLOCK_SIZE) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to read from /dev/urandom\n");
        continue;
      }
      if (write(socketfh, rbuf, MAX_BLOCK_SIZE) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to write to UDP socket. Reason: %d\n", errno);
      } else {
        sent_bytes += MAX_BLOCK_SIZE;
      }
      send_bytes += MAX_BLOCK_SIZE;
      clock_gettime(CLOCK_REALTIME, &clock);
      currenttime = clock.tv_sec;
    }
    logMessage(LOG_INFO_LEVEL, "Tried to send %ld, sent %ld\n", send_bytes, sent_bytes);
  }
  free(rbuf);
}

void log_request_start() {
  clock_gettime(CLOCK_REALTIME, &tstart);
  char msg[128];
  sprintf(msg, "START: %ld\n", BILLION * tstart.tv_sec + tstart.tv_nsec);
  write(timing_logfh, msg, strlen(msg));
}