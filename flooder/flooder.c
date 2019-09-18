//
// Created by sps5394 on 11/7/18.
//
#include <fcntl.h>
#include <sys/socket.h>
#include "flooder.h"
struct timespec tstart = {0, 0};

flooder_socks *flooder_create(char *addr, int port, char *client_addr, int client_port, int scale, int with_control)
{
  //
  //
  // LOCAL VARIABLES
  //
  //
  flooder_socks *socks = malloc(sizeof(flooder_socks));
  pthread_t flooder_check;
  socks->scale = scale;
  socks->with_control = with_control;
  logMessage(LOG_INFO_LEVEL, "Writing to UDP socket: %s:%d", addr, port);
  if ((socks->udp_sock = cmpsc311_client_connect_udp((unsigned char *)addr, (unsigned short)port)) == -1)
  {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return NULL;
  }
  if ((socks->client_sock = cmpsc311_client_connect((unsigned char *)client_addr, (unsigned short)client_port)) == -1)
  {
    logMessage(LOG_ERROR_LEVEL, "Could not connect to client\n");
    return NULL;
  }
  // Setup timing log
  if ((timing_logfh = open(TIME_LOG_NAME, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) ==
      -1)
  {
    // Error out
    logMessage(LOG_ERROR_LEVEL, "Error opening log : %s (%s)", TIME_LOG_NAME, strerror(errno));
    return NULL;
  }
  pthread_create(&flooder_check, NULL, (void *(*)(void *))flooder_test_connection, &socks->client_sock);
  pthread_detach(flooder_check);
  return socks;
}

void *flooder_test_connection(const int *socket_fd)
{
  void *buf = malloc(1);
  while (1)
  {
    read(*socket_fd, buf, 1);
    sleep(5);
    logMessage(LOG_INFO_LEVEL, "Closing flooder\n");
    exit(5);
  }
}

int flooder_run(flooder_socks *socks)
{
  //
  //
  // LOCAL VARIABLES
  //
  //
  int data_size = BLOCK_SIZE * socks->scale;
  char *rbuf = malloc(data_size);
  char client_start[6] = "START";
  char client_end[6] = "ENDIN";
  char client_control[6] = "CONTRO";
  int rfh, notified_control = 0;
  time_t clocktime, currenttime, start_time;
  struct timespec clock = {0, 0};
  long send_bytes = 0, sent_bytes = 0;

  if ((rfh = open("/dev/urandom", O_RDONLY, S_IRUSR)) == -1)
  {
    logMessage(LOG_ERROR_LEVEL, "Could not open /dev/urandom\n");
    return -1;
  }
  if (read(rfh, rbuf, data_size) == -1)
  {
    logMessage(LOG_ERROR_LEVEL, "Failed to read from /dev/urandom\n");
    return -1;
  }
  clock_gettime(CLOCK_MONOTONIC, &clock);
  start_time = clock.tv_sec;
  while (1)
  {
    sleep(5);
    logMessage(LOG_INFO_LEVEL, "Notifying start to client\n");
    write(socks->client_sock, client_start, 6);
    clock_gettime(CLOCK_MONOTONIC, &clock);
    clocktime = clock.tv_sec;
    currenttime = clocktime;
    logMessage(LOG_INFO_LEVEL, "Running flooder\n");
    log_request_start();
    send_bytes = 0;
    sent_bytes = 0;
    if (socks->with_control && currenttime - start_time >= 60 && !notified_control)
    {
      logMessage(LOG_INFO_LEVEL, "Starting control sequence\n");
      write(socks->client_sock, client_control, 6);
      notified_control = 1;
    }
    while (currenttime < clocktime + 1)
    {
      if (socks->with_control && notified_control)
      {
        clock_gettime(CLOCK_MONOTONIC, &clock);
        currenttime = clock.tv_sec;
        continue;
      }
      if (write(socks->udp_sock, rbuf, data_size) == -1)
      {
        logMessage(LOG_ERROR_LEVEL, "Failed to write to UDP socket. Reason: %s\n", strerror(errno));
      }
      else
      {
        sent_bytes += data_size;
      }
      send_bytes += data_size;
      clock_gettime(CLOCK_MONOTONIC, &clock);
      currenttime = clock.tv_sec;
    }
    logMessage(LOG_INFO_LEVEL, "Notifying end to client\n");
    write(socks->client_sock, client_end, 6);
    logMessage(LOG_INFO_LEVEL, "Tried to send %ld, sent %ld\n", send_bytes, sent_bytes);
  }
  free(rbuf);
}

void log_request_start()
{
  clock_gettime(CLOCK_MONOTONIC, &tstart);
  char msg[128];
  sprintf(msg, "START: %ld\n", BILLION * tstart.tv_sec + tstart.tv_nsec);
  write(timing_logfh, msg, strlen(msg));
}