//
// Created by sps5394 on 11/7/18.
//
#include <fcntl.h>
#include "flooder.h"
struct timespec tstart = {0,0};

flooder_socks *flooder_create(char *addr, int port, char *client_addr, int client_port) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  flooder_socks *socks = malloc(sizeof(flooder_socks));
  logMessage(LOG_INFO_LEVEL, "Writing to UDP socket: %s:%d", addr, port);
  if ((socks->udp_sock = cmpsc311_client_connect_udp((unsigned char *) addr, (unsigned short) port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return NULL;
  }
  if ((socks->client_sock = cmpsc311_client_connect((unsigned char *) client_addr, (unsigned short) client_port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not connect to client\n");
    return NULL;
  }
  // Setup timing log
  if (( timing_logfh = open(TIME_LOG_NAME, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) ==
      -1) {
    // Error out
    logMessage(LOG_ERROR_LEVEL, "Error opening log : %s (%s)", TIME_LOG_NAME, strerror(errno));
    return NULL;
  }
  return socks;
}

int flooder_run(flooder_socks *socks) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  char *rbuf = malloc(BLOCK_SIZE);
  char *client_start = "START";
  char *client_end = "ENDIN";
  int rfh;
  time_t clocktime, currenttime;
  struct timespec clock = {0, 0};
  long send_bytes = 0, sent_bytes = 0;

  if ((rfh = open("/dev/urandom", O_RDONLY, S_IRUSR)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not open /dev/urandom\n");
    return -1;
  }
  if (read(rfh, rbuf, BLOCK_SIZE) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Failed to read from /dev/urandom\n");
    return -1;
  }
  while (1) {
    sleep(5);
    write(socks->client_sock, client_start, strlen(client_start));
    clock_gettime(CLOCK_REALTIME, &clock);
    clocktime = clock.tv_sec;
    currenttime = clocktime;
    logMessage(LOG_INFO_LEVEL, "Running flooder\n");
    log_request_start();
    send_bytes = 0;
    sent_bytes = 0;
    while (currenttime < clocktime + 5) {
      if (write(socks->udp_sock, rbuf, BLOCK_SIZE) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to write to UDP socket. Reason: %d\n", errno);
      } else {
        sent_bytes += BLOCK_SIZE;
      }
      send_bytes += BLOCK_SIZE;
      clock_gettime(CLOCK_REALTIME, &clock);
      currenttime = clock.tv_sec;
    }
    write(socks->client_sock, client_end, strlen(client_end));
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