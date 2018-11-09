#ifndef CMPSC311_NETWORK_INCLUDED
#define CMPSC311_NETWORK_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_network.h
//  Description   : This is the network definitions for  the CRUD simulator.
//
//  Author        : Patrick McDaniel
//  Last Modified : Thu Oct 30 06:59:59 EDT 2014
//

// Include Files
#include <stdint.h>

// Defines

//
// Type Definitions

// 
// Network utility functions

int cmpsc311_connect_server( unsigned short port );
	// This function makes a server connection on a bound port.

int cmpsc311_accept_connection( int server );
	// This function accepts an incoming connection from a client (using the server socket)

int cmpsc311_client_connect(unsigned char *ip, uint16_t port);
	// Connect a client socket for network communication.

int cmpsc311_send_bytes( int sock, int len, unsigned char *block );
    // Send some bytes over the network

int cmpsc311_read_bytes( int sock, int len, unsigned char *block );
    // Read some bytes from the network

int cmpsc311_wait_read( int sock );
    // Wait until the socket has bytes to read

int cmpsc311_close( int sock );
	// Close a socket associated with network communication.

void cmpsc311_signal_handler( int no );
    // A generic break signal handler

//
// Unit Tests

int cmpsc311NetworkUnitTest( void );
  // This is the unit test for the networking code.

#endif
