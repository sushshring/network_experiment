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

#define FLOODER_ARGUMENTS "hvl:t:"
#define USAGE \
  "USAGE: flooder [-h] [-v] [-l <logfile>] [-t <flooder-type>] <packet-sink-address>\n" \
  "\n" \
  "where:\n" \
  "    -h - help mode (display this message)\n" \
  "    -v - verbose output\n" \
  "    -l - write log messages to the filename <logfile>\n" \
  "    -t - Type of flooder. 1: Self loop; 2: packet sink\n" \
  "\n" \
  "    <packet-sink-address> - The address of the packet sink. In case of self loop, " \
  "this will always be overriden to be 127.0.0.1\n" \
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
  int ch, verbose = 0, log_initialized = 0, socketfh = 0, flooder_type = 1;

  // Process command line parameters
  while (( ch = getopt(argc, argv, FLOODER_ARGUMENTS)) != -1) {
    switch (ch) {
      case 'h': // Help, print usage
        fprintf(stderr, USAGE);
        return ( -1 );
      case 'v':
        verbose = 1;
        break;
      case 'l':
        initializeLogWithFilename(optarg);
        log_initialized = 1;
        break;
      case 't':
        flooder_type = atoi(optarg);
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
  if (optind + 1>= argc) {
    // No filename
    fprintf(stderr, "Missing command line parameters, use -h to see usage, aborting.\n");
    return ( -1 );
  }

  if (( socketfh = flooder_create(argv[optind], atoi(argv[optind + 1]), flooder_type)) == -1 ) {
    logMessage(LOG_ERROR_LEVEL, "flooder failed to connect\n");
    return ( -1 );
  }

  if (flooder_run(socketfh) == -1) {
    logMessage(LOG_ERROR_LEVEL, "flooder had error\n");
    return ( -1 );
  }
}
