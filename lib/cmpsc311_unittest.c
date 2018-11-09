////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_unittest.h
//  Description   : This is the master unit test code for the 311 library.
//
//  Author   : Patrick McDaniel
//  Created  : Mon May  4 06:42:29 PDT 2015
//

// Include files
#include <cmpsc311_cache.h>
#include <cmpsc311_hashtable.h>
#include <cmpsc311_log.h>
#include <cmpsc311_network.h>
#include <cmpsc311_opcode.h>
#include <cmpsc311_util.h>
#include <cmpsc311_unittest.h>

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_unittests
// Description  : This is the main function calling all of the unit tests
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure


int cmpsc311_unittests(void) {

	// Run all of the tests
	if (cmpsc311CacheUnitTest() ||
			hashTableUnitTest() ||
			cmpsc311NetworkUnitTest() ||
			cmpsc311OpCodeUnitTest() ||
			b64UnitTest()) {
		logMessage(LOG_ERROR_LEVEL, "Unit tests failed for CMPSC311 library.\n");
	}

	// Return successfully
	return(0);
}

