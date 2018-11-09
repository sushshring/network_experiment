///////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_capture.c
//  Description   : This is an code for capturing the environment used to
//                  develop a program.  It is used for tracking use/development.
//
//   Author        : Devin Pholy
//   Last Modified : Tue Jun  9 07:38:49 EDT 2015
//   By            : Patrick McDaniel
//
//   ** Note: I had a lot of unresolved symbols in eclipse.  There are two things
//   that I did to fix it.
//   1) go to project->properties->C/C++ General->Paths and Symbols
// 		- enter the directories with the include files
//   2) go to project->properties->Code Analysis->Launching
//      - make sure that ONLY the run with build is checked

// Include Files
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Project Includes
#include <cmpsc311_capture.h>
#include <cmpsc311_log.h>

// Defines
#define CMPSC311_CAPTURE_DEFAULT_TARGET "libcmpsc311.a"

//
// Global Data
static char *log_target = CMPSC311_CAPTURE_DEFAULT_TARGET;

// Type definition for response
struct response {
	char *data;
	size_t size;
};

// Record for displaying results
struct record {
	int32_t tm, tz;
	uint32_t uid, gid;
	char user[256];
	char group[256];
	char host[256];
	uint8_t ip[4];
	uint32_t next;
};

//
// Functions

///////////////////////////////////////////////////////////////////////////////
//
// Function     : environment_capture_cb
// Description  : Callback for the capture of the environment
//
// Inputs       : ptr - data returned from the source
//                size - the size of each member
//                nmemb - number of members
//                userdata - callback structure (response)
// Outputs      : size of the structured data

static size_t environment_capture_cb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	// Recast the structure to our internal structure
	struct response *resp = userdata;

	// Resize the data to include the member region
	resp->data = realloc(resp->data, resp->size + size * nmemb + 1);
	if (!resp->data) {
		return 0;
	}

	// Copy the values into the reallocated data
	memcpy(&resp->data[resp->size], ptr, size * nmemb);
	resp->size += size * nmemb;
	resp->data[resp->size] = '\0';

	// Return the total size of the structure
	return(size * nmemb);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : environment_capture
// Description  : Capture the current environment and log
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int environment_capture(void) {

	// Local variables
	struct passwd *pwd;
	struct group *grp;
	char host[256];
	char *c;
	int fd;
	uint32_t ofs;
	uint8_t len;

	// Get timestamp, convert
	time_t t = time(NULL);
	int32_t tm = t;
	struct tm *ts = localtime(&t);
	int32_t tz = ts->tm_gmtoff;

	// Get user/group info
	uint32_t uid = geteuid();
	uint32_t gid = getegid();

	// Get the hostname of the local computer/VM (the best of our ability)
	if (gethostname(host, 256)) {
		host[0] = 0;
	} else {
		host[255] = 0;
	}

	// Get external IP
	in_addr_t addy = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();
	if (curl) {

		// Silent!
		FILE *devnull = fopen("/dev/null", "r+");
		if (devnull)
			curl_easy_setopt(curl, CURLOPT_STDERR, devnull);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);

		// Put it in the buffer
		struct response resp;
		resp.data = calloc(1, 1);
		resp.size = 0;

		// Setup the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, environment_capture_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &resp);

		// Get the page but don't take too long
		curl_easy_setopt(curl, CURLOPT_URL, "http://ipecho.net/plain");
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 2500L);

		// Go off and collect the data
		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
			addy = inet_addr(resp.data);

		// Clean up
		free(resp.data);
		curl_easy_cleanup(curl);
		fclose(devnull);
	}

	// Open file and append to end
	if ((fd=open(log_target, O_WRONLY, 0)) < 0) {
		return(-1);
	}
	ofs = lseek(fd, 0, SEEK_END); // Get to end of file

	// Standardize endianness
	tm = htonl(tm);
	tz = htonl(tz);
	uid = htonl(uid);
	gid = htonl(gid);
	ofs = htonl(ofs);

	// Print record
	write(fd, &tm, 4);
	write(fd, &tz, 4);
	write(fd, &uid, 4);
	write(fd, &gid, 4);

	// Get the login information, write to log
	pwd = getpwuid(ntohl(uid));
	len = pwd ? strlen(pwd->pw_name) : 0;
	write(fd, &len, 1);
	if (len) {
		for (c = pwd->pw_name; *c; c++) {
			*c ^= 0xca;
		}
		write(fd, pwd->pw_name, len);
	}

	// Get group information
	grp = getgrgid(ntohl(gid));
	len = grp ? strlen(grp->gr_name) : 0;
	write(fd, &len, 1);
	if (len) {
		for (c = grp->gr_name; *c; c++) {
			*c ^= 0xca;
		}
		write(fd, grp->gr_name, len);
	}

	// Finally, write hostname
	len = strlen(host);
	write(fd, &len, 1);
	if (len) {
		for (c = host; *c; c++) {
			*c ^= 0xca;
		}
		write(fd, host, len);
	}

	// Write booking information, and close the file
	write(fd, &addy, 4);
	write(fd, &ofs, 4);
	close(fd);

	// Return successfully
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : environment_capture
// Description  : Capture the current environment and log
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int display_environment_log(unsigned long lvl) {

	// Local variables
	struct record r;
	time_t local;
	uint32_t ofs, next;
	uint8_t len;
	char *c, *t;
	int fd, n;

	// Open file
	if ((fd = open(log_target, O_RDONLY)) < 0) {
		perror("open");
		return 1;
	}

	// Find offset of first entry
	next = lseek(fd, 0, SEEK_END);
	do {

		// Read next record
		ofs = next;
		pread(fd, &next, 4, ofs - 4);
		next = htonl(next);
		// Sanity check (27 = minimum record length)
		// TODO: hard coding the record length is icky
		if ((next) && ((next > ofs) || ((next + 27) > ofs))) {
			// Error out
			fprintf(stderr, "Bad file\n");
			close(fd);
			return(-1);
		}

	} while (next);  // Until reaching end of record

	// Go to first entry and start reading (with the time)
	lseek(fd, ofs, SEEK_SET);
	while ((n = read(fd, &r.tm, 4)) == 4) {

		// Read the time zone information, check result
		if (read(fd, &r.tz, 4) < 4) {
			perror("read tz");
			close(fd);
			return(-1);
		}

		// Read the user ID
		if (read(fd, &r.uid, 4) < 4) {
			perror("read uid");
			close(fd);
			return(-1);
		}

		// Read the group ID
		if (read(fd, &r.gid, 4) < 4) {
			perror("read gid");
			close(fd);
			return(-1);
		}

		// Read the user length (in characters)
		if (read(fd, &len, 1) < 1) {
			perror("read user len");
			close(fd);
			return(-1);
		}

		// Read the user name, unmask
		if (read(fd, &r.user, len) < len) {
			perror("read user");
			close(fd);
			return(-1);
		}
		for (c = r.user; c < r.user + len; c++)
			*c ^= 0xca;
		r.user[len] = '\0';

		// Read the group name (in characters)
		if (read(fd, &len, 1) < 1) {
			perror("read group len");
			close(fd);
			return(-1);
		}

		// Read the group name, unmask
		if (read(fd, &r.group, len) < len) {
			perror("read group");
			close(fd);
			return(-1);
		}
		for (c = r.group; c < r.group + len; c++)
			*c ^= 0xca;
		r.group[len] = '\0';

		// Read the hostname length
		if (read(fd, &len, 1) < 1) {
			perror("read host len");
			close(fd);
			return(-1);
		}

		// Read the host name, unmask
		if (read(fd, &r.host, len) < len) {
			perror("read host");
			close(fd);
			return(-1);
		}
		for (c = r.host; c < r.host + len; c++)
			*c ^= 0xca;
		r.host[len] = '\0';

		// Read the IP address
		if (read(fd, &r.ip, 4) < 4) {
			perror("read ip");
			close(fd);
			return(-1);
		}

		// Read offset of next record
		if (read(fd, &r.next, 4) < 4) {
			perror("read next offset");
			close(fd);
			return(-1);
		}

		// Fix endianness
		r.tm = ntohl(r.tm);
		r.tz = ntohl(r.tz);
		r.uid = ntohl(r.uid);
		r.gid = ntohl(r.gid);
		r.next = ntohl(r.next);

		// Print record
		local = r.tm + r.tz;
		t = asctime(gmtime(&local));
		t[strlen(t) - 1] = '\0';
		/*
		printf("%s\t%+03d%02d\t%s:%s\t%d:%d\t%s\t%u.%u.%u.%u\n",
				t, r.tz / 3600, (r.tz % 3600) / 60,
				r.user, r.group, r.uid, r.gid,
				r.host, r.ip[0], r.ip[1], r.ip[2], r.ip[3]);
		*/
		logMessage(lvl, "%s\t%+03d%02d\t%s:%s\t%d:%d\t%s\t%u.%u.%u.%u\n",
				t, r.tz / 3600, (r.tz % 3600) / 60,
				r.user, r.group, r.uid, r.gid,
				r.host, r.ip[0], r.ip[1], r.ip[2], r.ip[3]);
	}

	// Check to make sure we were able to read the record (not split record)
	if (n < 0) {
		perror("read tm");
		close(fd);
		return(-1);
	}

	// Close normally and return
	close(fd);
	return(0);
}

