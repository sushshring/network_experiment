//
// Created by sps5394 on 11/7/18.
//
#ifndef __linux__
// CLION IMPORTS
#include <strings.h>
#include <getopt.h>

#endif

// End CLION imports
#include <cmpsc311_log.h>
#include <unistd.h>
#include <cmpsc311_util.h>
#include "flooder.h"

#define FLOODER_ARGUMENTS "hvnl:s:t:c"
#define USAGE \
  "USAGE: flooder [-h] [-v] [-n] [-c] [-l <logfile>] [-t <flooder-type>] [-s <scale-factor>] <packet-sink-address> <packet-sink-port> <client-address> <client-port>\n" \
  "\n" \
  "where:\n" \
  "    -h - help mode (display this message)\n" \
  "    -v - verbose output\n" \
  "    -n - Ignore all scaling and do not run any flooding. Will force the flooder to use spin wait instead of sleep" \
  "    -l - write log messages to the filename <logfile>\n" \
  "    -t - Type of flooder. 1: Self loop; 2: packet sink\n" \
  "    -s - Scale factor to adjust flooding size\n" \
  "    -c - Run with a control sequence where no flooding is performed after one minute\n" \
  "\n" \
  "    <packet-sink-address> - The address of the packet sink. In case of self loop, " \
  "this will always be overriden to be 127.0.0.1\n" \
  "    <client-address> - The address of the client to notify flooder status\n" \
  "    <client-port> - The port of the client to notify flooder status\n" \
  "\n"

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the flooder
//
// Inputs       : argc - the number of command line parameters
//                argv - the parameters
// Outputs      : 0 if successful, -1 if failure
int main(int argc, char *argv[]) {
  // Local variables
  int ch, verbose = 0, log_initialized = 0, flooder_type = 1, with_control = 0;
  double flooder_scale = 1;
  flooder_socks *socks;

  // Process command line parameters
  while (( ch = getopt(argc, argv, FLOODER_ARGUMENTS)) != -1) {
    switch (ch) {
      case 'h': // Help, print usage
        fprintf(stderr, USAGE);
        return ( -1 );
      case 'v':
        verbose = 1;
        break;
      case 'c':
        with_control = 1;
        break;
      case 'n':
        flooder_type = -1;
      case 'l':
        initializeLogWithFilename(optarg);
        log_initialized = 1;
        break;
      case 's':
        flooder_scale = atof(optarg);
        break;
      default:
        fprintf(stderr, "Unknown command line option (%c), aborting.\n", ch);
        return ( -1 );
    }
  }
  // Setup the log as needed
  if (!log_initialized) {
    initializeLogWithFilehandle(CMPSC311_LOG_STDERR);
  }
  if (verbose) {
    enableLogLevels(LOG_INFO_LEVEL);
  }
  // The flooder address should be the next option
  if (optind + 3>= argc) {
    // No filename
    fprintf(stderr, "Missing command line parameters, use -h to see usage, aborting.\n");
    return ( -1 );
  }

  if (( socks = flooder_create(argv[optind], atoi(argv[optind + 1]), argv[optind+2], atoi(argv[optind + 3]), flooder_scale, with_control, flooder_type)) == NULL ) {
    logMessage(LOG_ERROR_LEVEL, "flooder failed to connect\n");
    return ( -1 );
  }

  if (flooder_run(socks) == -1) {
    logMessage(LOG_ERROR_LEVEL, "flooder had error\n");
    return ( -1 );
  }
}
