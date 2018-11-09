////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_network.c
//  Description   : This is the basic networking IO for the application.
//
//  Author        : Patrick McDaniel
//  Last Modified : Wed Nov  5 09:11:36 PST 2014
//

// Include Files
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Project Include Files
#include <cmpsc311_log.h>
#include <cmpsc311_network.h>
#include <cmpsc311_util.h>

// Defines
#define CMPSC311_MAX_BACKLOG 5
#define CMPSC311_UTEST_SERVER_PORT 17789
#define CMPSC311_UTEST_SERVER_ADDRESS (unsigned char *)"127.0.0.1"
#define CMPSC311_NETWORK_ITERATIONS 100
#define CMPSC311_NETWORK_UNIT_TEST_MAX_MSG_SIZE 4096

// Global variables
int cmpsc311_network_shutdown = 0;              // Flag indicating shutdown
unsigned char *cmpsc311_network_address = NULL; // Address of CMPSC311 server
unsigned short cmpsc311_network_port = 0;       // Port of CMPSC311 server

// Functional Prototypes (local methods)
void *cmpsc311NetworkUnitTestClient(void *arg);
int cmpsc311NetworkUnitSend(int sock);
int cmpsc311NetworkUnitRecv(int sock);

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_connect_server
// Description  : This function makes a server connection on a bound port.
//
// Inputs       : none
// Outputs      : the server socket if successful, -1 if failure

int cmpsc311_connect_server(unsigned short port) {

	// Local variables
	struct sigaction new_action;
	struct sockaddr_in saddr;
	int server, optval;

	// Set the signal handler
	new_action.sa_handler = cmpsc311_signal_handler;
	new_action.sa_flags = SA_NODEFER | SA_ONSTACK;
	sigaction(SIGINT, &new_action, NULL);

	// Create the socket
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 socket() create failed : [%s]", strerror(errno));
		return (-1);
	}

	// Setup so we can reuse the address
	optval = 1;
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 set socket option create failed : [%s]", strerror(errno));
		close(server);
		return (-1);
	}

	// Setup address and bind the server to a particular port
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Now bind to the server socket
	if (bind(server, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 bind() on port %u : [%s]", port, strerror(errno));
		close(server);
		server = -1;
		return (-1);
	}
	logMessage(LOG_INFO_LEVEL, "Server bound and listening on port [%d]", port);

	// Listen for incoming connection
	if (listen(server, CMPSC311_MAX_BACKLOG) == -1) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 listen() create failed : [%s]", strerror(errno));
		close(server);
		server = -1;
		return (-1);
	}

	// Return the socket
	return (server);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_accept_connection
// Description  : This function accepts an incoming connection from a client
//                (using the server socket)
//
// Inputs       : server - the server socket to accept the connection from
// Outputs      : the client socket if successful, -1 if failure

int cmpsc311_accept_connection(int server) {

	// Local variables
	struct sockaddr_in caddr;
	int client;
	unsigned int inet_len;

	// Accept the connection, error out if failure
	inet_len = sizeof(caddr);
	if ((client = accept(server, (struct sockaddr *)&caddr, &inet_len)) == -1) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 server accept failed, aborting [%s].", strerror(errno));
		return (-1);
	}

	// Log the creation of the new connection, return the new client connection
	logMessage(LOG_INFO_LEVEL, "Server new client connection [%s/%d]",
	           inet_ntoa(caddr.sin_addr), caddr.sin_port);
	return (client);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_client_connect
// Description  : Connect a client socket for network communication.
//
// Inputs       : ip - the IP address string for the client
//                port - the port number of the service
// Outputs      : socket file handle if successful, -1 if failure

int cmpsc311_client_connect(unsigned char *ip, uint16_t port) {

	// Local variables
	int sock;
	struct sockaddr_in caddr;

	// Check to make sure you have a good IP address
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	if (inet_aton((char *)ip, &caddr.sin_addr) == 0) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client unable to interpret IP address [%s]", ip);
		return (-1);
	}

	// Create the socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client socket() failed : [%s]", strerror(errno));
		return (-1);
	}

	// Now connect to the server
	if (connect(sock, (const struct sockaddr *)&caddr, sizeof(struct sockaddr)) == -1) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client connect() failed : [%s]", strerror(errno));
		return (-1);
	}

	// Return the socket
	return (sock);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_client_connect_udp
// Description  : Connect a client socket for network communication over UDP
//
// Inputs       : ip - the IP address string for the client
//                port - the port number of the service
// Outputs      : socket file handle if successful, -1 if failure

int cmpsc311_client_connect_udp(unsigned char *ip, uint16_t port) {

	// Local variables
	int sock;
	struct sockaddr_in caddr;

	// Check to make sure you have a good IP address
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	if (inet_aton((char *)ip, &caddr.sin_addr) == 0) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client unable to interpret IP address [%s]", ip);
		return (-1);
	}

	// Create the socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client socket() failed : [%s]", strerror(errno));
		return (-1);
	}

	// Now connect to the server
	if (connect(sock, (const struct sockaddr *)&caddr, sizeof(struct sockaddr)) == -1) {
		// Error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client connect() failed : [%s]", strerror(errno));
		return (-1);
	}

	// Return the socket
	return (sock);
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_send_bytes
// Description  : Send a specific length of bytes to socket
//
// Inputs       : sock - the socket filehandle of the clinet connection
//                len - the number of bytes to send
//                buf - the buffer to send
// Outputs      : 0 if successful, -1 if failure

int cmpsc311_send_bytes(int sock, int len, unsigned char *buf) {

	// Local variables
	int sentBytes = 0, sb;

	// Loop until you have read all the bytes
	while (sentBytes < len) {
		// Read the bytes and check for error
		if ((sb = write(sock, &buf[sentBytes], len - sentBytes)) < 0) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 send bytes failed : [%s]", strerror(errno));
			return (-1);
		}

		// Check for closed file
		else if (sb == 0) {
			// Close file, not an error
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 client socket closed on snd : [%s]", strerror(errno));
			return (-1);
		}

		// Now process what we read
		sentBytes += sb;
	}

	// Return successfully
	return (0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_read_bytes
// Description  : Receive a specific length of bytes from socket
//
// Inputs       : sock - the socket filehandle of the client connection
//                len - the number of bytes to be read
//                buf - the buffer to read into
// Outputs      : 0 if successful, -1 if failure

int cmpsc311_read_bytes(int sock, int len, unsigned char *buf) {

	// Local variables
	int readBytes = 0, rb;

	// Loop until you have read all the bytes
	while (readBytes < len) {
		// Read the bytes and check for error
		if ((rb = read(sock, &buf[readBytes], len - readBytes)) < 0) {
			// Check for client error on read
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 read bytes failed : [%s]", strerror(errno));
			return (-1);
		}

		// Check for closed file
		else if (rb == 0) {
			// Close file, not an error
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 client socket closed on rd : [%s]", strerror(errno));
			return (-1);
		}

		// Now process what we read
		readBytes += rb;
	}

	// Return successfully
	return (0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_wait_read
// Description  : Wait for user input on the socket
//
// Inputs       : sock - the socket to write on
// Outputs      : 0 if successful, -1 if failure

int cmpsc311_wait_read(int sock) {

	// Local variables
	fd_set rfds;
	int nfds, ret;

	// Setup and perform the select
	nfds = sock + 1;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	ret = select(nfds, &rfds, NULL, NULL, NULL);

	// Check the return value
	if (ret == -1) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 select() failed : [%s]", strerror(errno));
		return (-1);
	}

	// check to make sure we are selected on the read
	if (FD_ISSET(sock, &rfds) == 0) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 select() returned without selecting FD : [%d]", sock);
		return (-1);
	}

	// Return successfully
	return (0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_signal_handler
// Description  : Close a socket associated with network communication.
//
// Inputs       : sock - the socket to close
// Outputs      : 0 if successful, -1 if failure

int cmpsc311_close(int sock) {

	// Log and return the close value
	logMessage(LOG_INFO_LEVEL, "CMPSC311 closing socket [%d]", sock);
	return (close(sock));
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311_signal_handler
// Description  : This is the user defined signal handler
//
// Inputs       : no - the signal number
// Outputs      : 0 if successful, -1 if failure

void cmpsc311_signal_handler(int no) {

	// Log the signal and set process to shut down
	logMessage(LOG_WARNING_LEVEL, "CMPSC311 signal received (%d), shutting down.", no);
	cmpsc311_network_shutdown = 1;

	// Return, no return code
	return;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311NetworkUnitTest
// Description  : This is the unit test for the networking code.
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int cmpsc311NetworkUnitTest(void) {

	// Local variables
	int server, i, client;
	pthread_t client_thread;

	// Connect the server
	if ((server = cmpsc311_connect_server(CMPSC311_UTEST_SERVER_PORT)) == -1) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 server connect failed : [%s]", strerror(errno));
		return (-1);
	}

	// Now do a number of client processing iterations
	for (i = 0; i < CMPSC311_NETWORK_ITERATIONS; i++) {

		// Create a thread to process the client data
		pthread_create(&client_thread, NULL, cmpsc311NetworkUnitTestClient, &i);

		// Wait for incoming connection, accept it
		cmpsc311_wait_read(server);
		if ((client = cmpsc311_accept_connection(server)) == -1) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 client connect failed : [%s]", strerror(errno));
			return (-1);
		}

		// Receive some random data, then send some
		if ((cmpsc311NetworkUnitRecv(client)) || (cmpsc311NetworkUnitSend(client))) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 unit test protocol failed (send/recv) : [%s]", strerror(errno));
			return (-1);
		}

		// Log and close the client connection
		logMessage(LOG_INFO_LEVEL, "CMPSC311 Network client/server unit test iteration complete");
		cmpsc311_close(client);

		// Now join the client thread
		pthread_join(client_thread, NULL);
	}

	// Close the server
	cmpsc311_close(server);

	// Return successfully
	logMessage(LOG_INFO_LEVEL, "Network unit test completed successfully.");
	return (0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311NetworkUnitTestClient
// Description  : This is the client thread function for the network unit test
//
// Inputs       : arg - arguments to the client function (should be NULL)
// Outputs      : pointer to a integer return value

void *cmpsc311NetworkUnitTestClient(void *arg) {

	// Local variables
	int *retval, client;

	// Create return value, set to success
	retval = malloc(sizeof(int));
	*retval = 0;

	// Log the starting of the test, connect to the server
	logMessage(LOG_INFO_LEVEL, "Starting CMPSC311 network client iteration test %d.", *(int *)arg);
	if ((client = cmpsc311_client_connect(CMPSC311_UTEST_SERVER_ADDRESS, CMPSC311_UTEST_SERVER_PORT)) == -1) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 client connect failed : [%s]", strerror(errno));
		*retval = -1;
		return (retval);
	}

	// Send, then receive some random data
	if ((cmpsc311NetworkUnitSend(client)) || (cmpsc311NetworkUnitRecv(client))) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 unit test protocol failed (send/recv) : [%s]", strerror(errno));
		*retval = -1;
		return (retval);
	}

	// Log and close the client connection
	logMessage(LOG_INFO_LEVEL, "CMPSC311 Network client/server unit test iteration complete");
	cmpsc311_close(client);

	// Return the return value and exit
	pthread_exit(retval);
	return (NULL); // Unreachable
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311NetworkUnitSend
// Description  : Perform a simple send
//
// Inputs       : sock - socket to perform test on
// Outputs      : 0 if successful, -1 if failure

int cmpsc311NetworkUnitSend(int sock) {

	// Local variables
	uint16_t len;
	char ch, buf[CMPSC311_NETWORK_UNIT_TEST_MAX_MSG_SIZE];

	// Create a randomized buffer to send
	len = getRandomValue(1, CMPSC311_NETWORK_UNIT_TEST_MAX_MSG_SIZE);
	ch = (char)getRandomValue(0, 255);
	memset(buf, ch, len);

	// Now send the length, the buffer and the
	if (cmpsc311_send_bytes(sock, sizeof(uint16_t), (unsigned char *)&len) ||
	    cmpsc311_send_bytes(sock, sizeof(char), (unsigned char *)&ch) ||
	    cmpsc311_send_bytes(sock, len, (unsigned char *)buf)) {
		// Failed, error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 network unit test data send fail : [%s]", strerror(errno));
		return (-1);
	}

	// Return successfully
	logMessage(LOG_INFO_LEVEL, "CMPSC311 network unit test message send : [len=%d,ch=%x]", len, ch);
	return (0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311NetworkUnitRecv
// Description  : Perform a simple receive
//
// Inputs       : sock - socket to perform test on
// Outputs      : 0 if successful, -1 if failure

int cmpsc311NetworkUnitRecv(int sock) {

	// Local variables
	uint16_t len, i;
	char ch, buf[CMPSC311_NETWORK_UNIT_TEST_MAX_MSG_SIZE];

	// Create a randomized buffer to send
	len = getRandomValue(1, CMPSC311_NETWORK_UNIT_TEST_MAX_MSG_SIZE);
	ch = (char)getRandomValue(0, 255);
	memset(buf, ch, len);

	// Now send the length, the buffer and the
	if (cmpsc311_read_bytes(sock, sizeof(uint16_t), (unsigned char *)&len) ||
	    cmpsc311_read_bytes(sock, sizeof(char), (unsigned char *)&ch) ||
	    cmpsc311_read_bytes(sock, len, (unsigned char *)buf)) {
		// Failed, error out
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 network unit test data read fail : [%s]", strerror(errno));
		return (-1);
	}

	// Now check to see of the memory is correct
	for (i = 0; i < len; i++) {
		if (buf[i] != ch) {
			// Failed, error out
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 network unit test mismatch : [%s]", strerror(errno));
			return (-1);
		}
	}

	// Log and return successfully
	logMessage(LOG_INFO_LEVEL, "CMPSC311 network unit test message received : [len=%d,ch=%x]", len, ch);
	return (0);
}
