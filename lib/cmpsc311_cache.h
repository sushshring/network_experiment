#ifndef CMPSC311_CACHE_INCLUDED
#define CMPSC311_CACHE_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : cmpsc311_cache.h
//  Description    : This is the header file for the implementation of the
//                   cache for the cmpsc311 generic driver code.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Sun Nov 30 09:30:46 EST 2014
//

// Includes
#include <stdint.h>

typedef uint32_t CacheID;       // Cache item value

///
// Cache Interfaces

int init_cmpsc311_cache(uint32_t max_items, uint32_t max_size);
	// Initialize the cache and place maximum sizes/elements

int close_cmpsc311_cache(void);
	// Clear all of the contents of the cache, cleanup

int put_cmpsc311_cache(CacheID cid, void *buf, uint32_t len);
	// Put an object into the object cache

void * get_cmpsc311_cache(CacheID cid);
	// Get an object from the cache (and return it)

void * delete_cmpsc311_cache(CacheID cid);
	// Remove and object from the cache (and return it)

//
// Unit test

int cmpsc311CacheUnitTest(void);
	// Run a unit test checking the cache implementation

#endif
