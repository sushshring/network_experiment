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
#include "sink.h"

#define SINK_ARGUMENTS "hvl:t:"
#define USAGE \
  "USAGE: sink [-h] [-v] [-l <logfile>] <packet-sink-port>\n" \
  "\n" \
  "where:\n" \
  "    -h - help mode (display this message)\n" \
  "    -v - verbose output\n" \
  "    -l - write log messages to the filename <logfile>\n" \
  "\n" \
  "    <packet-sink-port> - The port of the packet sink\n" \
  "\n"

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the sink
//
// Inputs       : argc - the number of command line parameters
//                argv - the parameters
// Outputs      : 0 if successful, -1 if failure
int main(int argc, char *argv[]) {
  // Local variables
  int ch, verbose = 0, log_initialized = 0, socketfh = 0;

  // Process command line parameters
  while (( ch = getopt(argc, argv, SINK_ARGUMENTS)) != -1) {
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
  // The sink address should be the next option
  if (optind >= argc) {
    // No filename
    fprintf(stderr, "Missing command line parameters, use -h to see usage, aborting.\n");
    return ( -1 );
  }

  if (( socketfh = sink_create(atoi(argv[optind]))) == -1 ) {
    logMessage(LOG_ERROR_LEVEL, "sink failed to connect\n");
    return ( -1 );
  }

  if (sink_run(socketfh) == -1) {
    logMessage(LOG_ERROR_LEVEL, "sink had error\n");
    return ( -1 );
  }
}
