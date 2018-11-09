////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_uncapture.c
//  Description   : The uncapture utility reads the contents of the previously
//                  written environmental information and displays it.
//
//   Author        : Devin Pholy
//   Last Modified : Tue Jun  9 07:38:49 EDT 2015
//   By            : Patrick McDaniel
//


// Include files
#include <getopt.h>

// Project Includes
#include <cmpsc311_log.h>
#include <cmpsc311_capture.h>

// Defines
#define UNCAP311_ARGUMENTS "hl:"
#define USAGE \
	"USAGE: capread311 [-h] [-l <logfile>]\n" \
	"\n" \
	"where:\n" \
	"    -h - help mode (display this message)\n" \
	"    -l - write log messages to the filename <logfile>\n" \
	"\n" \

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the CRUD simulator
//
// Inputs       : argc - the number of command line parameters
//                argv - the parameters
// Outputs      : 0 if successful test, -1 if failure

int main( int argc, char *argv[] ) {
	// Local variables
	int ch, log_initialized = 0;

	// Process the command line parameters
	while ((ch = getopt(argc, argv, UNCAP311_ARGUMENTS)) != -1) {

		switch (ch) {
		case 'h': // Help, print usage
			fprintf( stderr, USAGE );
			return( -1 );

		case 'l': // Set the log filename
			initializeLogWithFilename(optarg);
			log_initialized = 1;
			break;

		default:  // Default (unknown)
			fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
			return( -1 );
		}
	}

	// Setup the log as needed
	if (!log_initialized) {
		initializeLogWithFilehandle(CMPSC311_LOG_STDERR);
	}
	enableLogLevels(LOG_INFO_LEVEL);

	// Print out the log capture
	if (display_environment_log(LOG_INFO_LEVEL)) {
		logMessage(LOG_ERROR_LEVEL, "311 capture log read failed.");
	}

	// Log and return successfully
	logMessage(LOG_INFO_LEVEL, "311 capture log read successful.");
	return( 0 );
}

