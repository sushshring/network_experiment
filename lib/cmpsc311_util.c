////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_util.c
//  Description   : This is a set of general-purpose utility functions we use
// 					for the 311 homework assignments.
//
//  Author        : Patrick McDaniel
//  Created       : Sat Sep 21 06:47:40 EDT 2013
//
//
//  Change Log:
//
//  10/11/13    Added the timer comparison function definition (PDM)

// System include files
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <gcrypt.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>

// Project Include Files
#include <cmpsc311_util.h>
#include <cmpsc311_log.h>

//
// Defines

#define CMPSC_BSWAP64(val) \
	(((val & 0x00000000000000ff) << 56) | \
	 ((val & 0x000000000000ff00) << 40) | \
	 ((val & 0x0000000000ff0000) << 24) | \
	 ((val & 0x00000000ff000000) << 8) | \
	 ((val & 0x000000ff00000000) >> 8) | \
	 ((val & 0x0000ff0000000000) >> 24) | \
	 ((val & 0x00ff000000000000) >> 40) | \
	 ((val & 0xff00000000000000) >> 56))

//
// Global data

int gcrypt_initialized = 0;  // Flag indicating the library needs to be initialized
gcry_md_hd_t *hfunc = NULL;  // A pointer to the gcrypt hash structure

//
// Local functions
int init_gcrypt(void); // initialize the GCRYPT library

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : generate_md5_signature
// Description  : Generate MD5 signature from buffer
//
// Inputs       : buf - the buffer to generate the signature
//                size - the size of the buffer (in bytes)
//                sig - the signature buffer
//                sigsz - ptr to the size of the signature buffer
//                        (set to sig length when done)
// Outputs      : -1 if failure or 0 if successful

int generate_md5_signature(char *buf, uint32_t size, char *sig, uint32_t *sigsz) {

	// Local variables
	unsigned char *hvalue;

	// Init as needed
	init_gcrypt();

	// Check the signature length
	if ( *sigsz < CMPSC311_HASH_LENGTH ) {
		logMessage( LOG_ERROR_LEVEL, "Signature buffer too short  [%d<%d]",	*sigsz, CMPSC311_HASH_LENGTH );
		return( -1 );
	}
	*sigsz = CMPSC311_HASH_LENGTH;

	// Perform the signature operation
	gcry_md_reset( *hfunc );
	gcry_md_write( *hfunc, buf, size );
	gcry_md_final( *hfunc );
	hvalue = gcry_md_read( *hfunc, 0 );

	// Check the result
	if ( hvalue == NULL ) {
		logMessage( LOG_ERROR_LEVEL, "Signature generation failed [NULL]" );
		return( -1 );
	}

	// Copy the signature bytes, return successfully
	memcpy( sig, hvalue, *sigsz );
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : bufToString
// Description  : Convert the buffer into a readable hex string
//
// Inputs       : buf - the buffer to make a string out of
//                blen - the length of the buffer
//                str - the place to put the string
//                slen - the length of the output string
// Outputs      : 0 if successful, -1 if failure

int bufToString(char *buf, uint32_t blen, char *str, uint32_t slen ) {

	// Variables and startup
	int i;
	char sbuf[25];
    str[0] = 0x0; // Null terminate the string

    // Now walk the bytes (up to a max 128 bytes)
    for (i=0; ((i<(int)blen)&&(i<128)&&(strlen((char *)str)+5<slen)); i++) {
        sprintf( sbuf, "0x%02x ", (unsigned char)buf[i] );
        strncat( (char *)str, (char *)sbuf, slen-strlen((char *)str) );
    }

	// Return successfully
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : getRandomValue
// Description  : Using strong randomness, generate random number
//
// Inputs       : min - the minimum number
//                max - the maximum number
// Outputs      : the random value

uint32_t getRandomValue( uint32_t min, uint32_t max ) {

	// Local variables
	uint32_t val;
    uint32_t range_length = max-min+1;
	init_gcrypt();
#if 0
	gcry_randomize( &val, sizeof(val), GCRY_STRONG_RANDOM );
#else
	gcry_randomize( &val, sizeof(val), GCRY_WEAK_RANDOM );
#endif
	// Adjust to range
    // range_length==0 when min=0 & max=UINT32_MAX due to integer overflow
	if ( range_length != 0 ) {
		val = (uint32_t)(val%range_length)+min;
	}
	return( val );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : getRandomData
// Description  : Using gcrypt randomness, generate random data
//
// Inputs       : blk - place to put the random data
//                sz - the size of the data
// Outputs      : the random value

int getRandomData( char *blk, uint32_t sz ) {

#if 0
	gcry_randomize( blk, sz, GCRY_STRONG_RANDOM );
#else
	gcry_randomize( blk, sz, GCRY_WEAK_RANDOM );
#endif
	return( 0 );
}
////////////////////////////////////////////////////////////////////////////////
//
// Function     : compareTimes
// Description  : Compare two timer values
//
// Inputs       : tm1 - the first time value
//                tm2 - the second time value
// Outputs      : <0, 0, <0 if tmr1 < tmr, tmr1==tmr2, tmr1 > tmr2 , respectively

long compareTimes( struct timeval * tm1, struct timeval * tm2 ) {

    // compute the difference in usec
    long retval = 0;
    if ( tm2->tv_usec < tm1->tv_usec ) {
	retval = (tm2->tv_sec-tm1->tv_sec-1)*1000000L;
	retval += ((tm2->tv_usec+1000000L)-tm1->tv_usec);
    } else {
	retval = (tm2->tv_sec-tm1->tv_sec)*1000000L;
	retval += (tm2->tv_usec-tm1->tv_usec);
    }
    return( retval );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : htonll64
// Description  : 64-bit host to network long
//
// Inputs       : val - the value to put into host byte order
// Outputs      : the byte converted value

uint64_t htonll64(uint64_t val) {

	// Setup some local variables
	uint32_t end = 1;
	char *ptr =  (char *)&end;

	// Lets check the endianess of the machine, swap if little endian
	if ( *ptr == 0x1 ) {
		return( CMPSC_BSWAP64(val) );
	}

	// Big endian, just return that
    return(val);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : htonll64
// Description  : 64-bit network to host long
//
// Inputs       : val - the value to put into network byte order
// Outputs      : the byte converted value

uint64_t ntohll64(uint64_t val) {

	// Setup some local variables
	uint32_t end = 1;
	char *ptr = (char *)&end;

	// Lets check the endianess of the machine, swap if little endian
	if ( *ptr == 0x1 ) {
		return( CMPSC_BSWAP64(val) );
	}

	// Big endian, just return that
    return(val);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : b64UnitTest
// Description  : 64-bit conversion unit test
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int b64UnitTest( void ) {

	// Local variables
	int i;
	uint64_t val, oval;

	// Repeat for a good while
	for (i=0; i<1000; i++) {
		// Pick a random number, convert
		val = ((uint64_t)getRandomValue(0,-1) << 32) | getRandomValue(0,-1);
		oval = ntohll64(htonll64(val));

		// Check to make sure it converted correctly
		if (val != oval) {
			// Error, bail out
			logMessage(LOG_ERROR_LEVEL, "b64 conversion unit test failed, [%lx != %lx]", val, oval);
			return(-1);
		}
	}

	// Log success and return
	logMessage(LOG_INFO_LEVEL, "b64 conversion unit test successful.");
	return(0);
}

//
// Local Functions
GCRY_THREAD_OPTION_PTHREAD_IMPL;

////////////////////////////////////////////////////////////////////////////////
//
// Function     : init_gcrypt
// Description  : initialize the GCRYPT library
//
// Inputs       : tm1 - the first time value
//                tm2 - the second time value
// Outputs      : 0 if successful, -1 if failure

int init_gcrypt( void ) {

	// If the GCRYPT interface not initialized
	if ( ! gcrypt_initialized ) {

		// gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pth);

		// Initialize the library
		gcry_error_t err;
		gcry_check_version( GCRYPT_VERSION );

		// Create the hash structure
		hfunc = malloc( sizeof(gcry_md_hd_t) );
		memset( hfunc, 0x0, sizeof(gcry_md_hd_t) );
		err = gcry_md_open( hfunc, CMPSC311_HASH_TYPE, 0 );
		if ( err != GPG_ERR_NO_ERROR  ) {
			logMessage( LOG_ERROR_LEVEL, "Unable to init hash algorithm  [%s]", gcry_strerror(err) );
			return( -1 );
		}

		// Set the initialized flag
		gcrypt_initialized = 1;
	}

	// Return successfully
	return(0);
}
