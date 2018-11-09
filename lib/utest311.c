////////////////////////////////////////////////////////////////////////////////
//
//  File           : utest311.c
//  Description    : This is the unit test program for the CMPSC311 programming
//                   assignment library.  It is used to test all of the functions
//                   of the library used to create the programs.
//
//   Author        : Patrick McDaniel
//   Last Modified : Sun Nov 30 08:53:46 EST 2014
//

// Include Files
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Project Includes
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <cmpsc311_hashtable.h>
#include <cmpsc311_cache.h>
#include <cmpsc311_network.h>
#include <cmpsc311_opcode.h>

// Defines
#define UTEST311_ARGUMENTS "hvul:x:a:p:"
#define USAGE \
	"USAGE: crud [-h] [-v] [-l <logfile>]\n" \
	"\n" \
	"where:\n" \
	"    -h - help mode (display this message)\n" \
	"    -v - verbose output\n" \
	"    -l - write log messages to the filename <logfile>\n" \
	"\n" \

//
// Global Data
int verbose;

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
	int ch, verbose = 0, log_initialized = 0;

	// Process the command line parameters
	while ((ch = getopt(argc, argv, UTEST311_ARGUMENTS)) != -1) {

		switch (ch) {
		case 'h': // Help, print usage
			fprintf( stderr, USAGE );
			return( -1 );

		case 'v': // Verbose Flag
			verbose = 1;
			break;

		case 'l': // Set the log filename
			initializeLogWithFilename( optarg );
			log_initialized = 1;
			break;

		default:  // Default (unknown)
			fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
			return( -1 );
		}
	}

	// Setup the log as needed
	if ( ! log_initialized ) {
		initializeLogWithFilehandle( CMPSC311_LOG_STDERR );
	}
	if ( verbose ) {
		enableLogLevels( LOG_INFO_LEVEL );
	}

	cmpsc311OpCodeUnitTest();
	return(0);

	// Run the unit tests here
	if ( (b64UnitTest()) ||
			(hashTableUnitTest()) ||
			(cmpsc311CacheUnitTest()) ||
			(cmpsc311NetworkUnitTest()) ||
			(cmpsc311OpCodeUnitTest())) {
		logMessage(LOG_ERROR_LEVEL, "libcmpsc311 unit tests failed.");
		return(-1);
	}

	// Log and return successfully
	logMessage(LOG_INFO_LEVEL, "Unit tests completed successfully.");
	return( 0 );
}

