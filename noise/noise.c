//
// Created by Sushrut Shringarputale on 2019-01-22.
//

#include "noise.h"

int noise_run(int socketfh) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int next_noise_amt = get_noise_bytes(noise_amt);
  char *rbuf = malloc((size_t) next_noise_amt);
  int rfh;
  time_t clocktime, currenttime;
  struct timespec clock = {0, 0};
  long send_bytes = 0, sent_bytes = 0;

  if ((rfh = open("/dev/urandom", O_RDONLY, S_IRUSR)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not open /dev/urandom\n");
    return -1;
  }
  pthread_t *sysinthread = malloc(sizeof(pthread_t));
  pthread_create(sysinthread, NULL, sysin_listener);
  while (1) {
    sleep(5);
    clock_gettime(CLOCK_REALTIME, &clock);
    clocktime = clock.tv_sec;
    currenttime = clocktime;
    logMessage(LOG_INFO_LEVEL, "Running noise\n");
//    log_request_start();
    send_bytes = 0;
    sent_bytes = 0;
    while (currenttime < clocktime + 5) {
      if (read(rfh, rbuf, next_noise_amt) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to read from /dev/urandom\n");
        continue;
      }
      if (write(socketfh, rbuf, next_noise_amt) == -1) {
        logMessage(LOG_ERROR_LEVEL, "Failed to write to UDP socket. Reason: %d\n", errno);
      } else {
        sent_bytes += next_noise_amt;
      }
      send_bytes += next_noise_amt;
      clock_gettime(CLOCK_REALTIME, &clock);
      currenttime = clock.tv_sec;
    }
    logMessage(LOG_INFO_LEVEL, "Tried to send %ld, sent %ld\n", send_bytes, sent_bytes);
    free(rbuf);
    next_noise_amt = get_noise_bytes(noise_amt);
    rbuf = malloc((size_t) next_noise_amt);;
  }
  free(rbuf);
}

int noise_connect(unsigned char *server_addr, uint16_t port) {
  //
  //
  // LOCAL VARIABLES
  //
  //
  int serv_sock;
  logMessage(LOG_INFO_LEVEL, "Writing to UDP socket: %s:%d", server_addr, port);
  if ((serv_sock = cmpsc311_client_connect_udp(server_addr, (unsigned short) port)) == -1) {
    logMessage(LOG_ERROR_LEVEL, "Could not listen\n");
    return -1;
  }
//  // Setup timing log
//  if (( timing_logfh = open(TIME_LOG_NAME, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) ==
//      -1) {
//    // Error out
//    logMessage(LOG_ERROR_LEVEL, "Error opening log : %s (%s)", TIME_LOG_NAME, strerror(errno));
//    return ( -1 );
//  }
  return serv_sock;
}

void *sysin_listener(void *param) {
  while (1) {
   char *readline;
   size_t linecap = 0;
   getline(&readline, &linecap, STDIN_FILENO);
   int size = atoi(readline);
   noise_amt = size;
  }
}

int get_noise_bytes(int input_parameter) {
  return input_parameter*2;
}
