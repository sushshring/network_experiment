#ifndef CMPSC311_CAPTURE_INCLUDED
#define CMPSC311_CAPTURE_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_capture.h
//  Description   : This is an interface for capturing the environment used to
//                  develop a program.  It is used for tracking use/development.
//
//  Author   : Patrick McDaniel
//  Created  : Tue May 12 07:08:30 EDT 2015
//

// Includes

// Defines

// Functional Prototypes

int environment_capture(void);
	// Capture the current environment and log

int display_environment_log(unsigned long lvl);
	// Print environment detail to a log

#endif // CMPSC311_UTIL_INCLUDED
