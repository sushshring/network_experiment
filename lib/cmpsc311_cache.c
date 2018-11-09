////////////////////////////////////////////////////////////////////////////////
//
//  File           : cmpsc311_cache.c
//  Description    : This is the implementation of the cache for the generic
//                   cmpsc311 driver library.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Sun Nov 30 09:36:02 EST 2014
//

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Project includes
#include <cmpsc311_cache.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <cmpsc311_hashtable.h>

// Defines
#define CMPSC311_CACHE_TABLE_BIT_WIDTH 8
#define CMPSC311_CACHE_UNIT_TEST_MAX_ITEMS 25
#define CMPSC311_CACHE_UNIT_TEST_MAX_SIZE 20400
#define CMPSC311_CACHE_UNIT_TEST_BINS 128
#define CMPSC311_CACHE_UNIT_TEST_ITERATIONS 50000
#define CMPSC311_CACHE_UNIT_TEST_MAX_OBJSIZE 1024
#define CMPSC311_CACHE_UNIT_TEST_STARTVAL 1024
#define CMPSC311_NO_CACHE_ID 0  // ID signifying empty object

// Cache structure
typedef struct cmpsc311_cache_entry {
	CacheID        cid;   // The cache ID of the buffer
	uint32_t       size;  // The size of the buffer
	void          *buf;   // The buffer
	struct timeval last_used;  // This is the time the cache entry was last used
} Cmpsc311CacheEntry;

// Structure for managing the unit test for the cache implementation
typedef struct {
	void     *buf;  // The buffer associated with this cache entry
	CacheID   cid;  // This is the CID of the test buffer
	uint32_t  size; // This is the size of the test buffer
} Cmpsc311CacheUnitTestRecord;

//
// Global Data

uint8_t  cmpsc311_cache_initialized = 0;    // Flag indicating the hash table has been initialized
uint32_t cmpsc311_cache_max_items = 0;      // Maximum number of items you can can keep in the cache
uint32_t cmpsc311_cache_max_size = 0;       // Maximum number of bytes you can can keep in the cache
uint32_t cmpsc311_cache_current_items = 0;  // THe current size of the cache (in items)
uint32_t cmpsc311_cache_current_size = 0;   // THe current size of the cache
HTable  *cmpsc311_htable = NULL;            // This is the hash table to hold the data items

// Local functional prototypes
int eject_cmpsc311_cache(void);  // Eject an item from the cache

//
// cmpsc311 interface

////////////////////////////////////////////////////////////////////////////////
//
// Function     : init_cmpsc311_cache
// Description  : Initialize the cache and place maximum sizes/elements
//
// Inputs       : max_items - the maximum number of items your cache can hold
//                max_size - the maximum size (in bytes) the  cache can hold
// Outputs      : 0 if successful, -1 if failure

int init_cmpsc311_cache(uint32_t max_items, uint32_t max_size) {

	// Check to see if already initialized
	if (cmpsc311_cache_initialized) {
		logMessage(LOG_ERROR_LEVEL, "init_cmpsc311_cache: initializing already initialized cache");
		return(-1);
	}

	// Check for sane maximum item value
	if (max_items == 0) {
		logMessage(LOG_ERROR_LEVEL, "init_cmpsc311_cache: max_items must be greater than zero");
		return(-1);
	}

	// Check for sane maximum size value
	if (max_size == 0) {
		logMessage(LOG_ERROR_LEVEL, "init_cmpsc311_cache: max_size must be greater than zero");
		return(-1);
	}

	// Setup the cache
	cmpsc311_cache_max_items = max_items;
	cmpsc311_cache_max_size = max_size;
	cmpsc311_cache_current_items = 0;
	cmpsc311_cache_current_size = 0;
	cmpsc311_htable = malloc(sizeof(HTable));
	initHashTable(cmpsc311_htable, CMPSC311_CACHE_TABLE_BIT_WIDTH);

	// Set to initialized, return successfully
	logMessage(LOG_INFO_LEVEL, "init_cmpsc311_cache: initialization complete [%u/%u]", max_items, max_size);
	logMessage(LOG_INFO_LEVEL, "Cache state [%u items, %u bytes used]", cmpsc311_cache_current_items,
				cmpsc311_cache_current_size);
	cmpsc311_cache_initialized = 1;
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : close_cmpsc311_cache
// Description  : Clear all of the contents of the cache, cleanup
//
// Inputs       : none
// Outputs      : o if successful, -1 if failure

int close_cmpsc311_cache(void) {

	// Local variables
	Cmpsc311CacheEntry *ref = NULL, *chk;
	HtIterator it;
	uint32_t dropped = cmpsc311_cache_current_items;

	// Check to see if not initialized
	if (!cmpsc311_cache_initialized) {
		logMessage(LOG_ERROR_LEVEL, "Error, trying to close and uninitialized cache");
		return(-1);
	}

	// Keep deleting until hash table empty
	while (cmpsc311_cache_current_items > 0) {

		// Setup iterator and get first item
		initHashTableIterator(cmpsc311_htable, &it);
		if((ref = iterateHashTable(&it)) == NULL) {
			logMessage(LOG_ERROR_LEVEL, "close_cmpsc311_cache: failed iterator in closing cache");
			return(-1);
		}

		// Now delete the item from the cache
		chk = deleteValueFromHashTable(cmpsc311_htable, ref->cid);
		if (ref != chk) {
			logMessage(LOG_ERROR_LEVEL, "close_cmpsc311_cache: iterate/delete mismatch [%p!=%p]", ref, chk);
			return(-1);
		}

		// Cleanup the cache line
		cmpsc311_cache_current_items --;
		cmpsc311_cache_current_size -= ref->size;
		free(ref->buf);
		free(ref);
	}

	// Now cleanup the hash table itself, delete
	cleanupHashTable(cmpsc311_htable);
	free(cmpsc311_htable);

	// Zero out the global data
	cmpsc311_htable = NULL;
	cmpsc311_cache_max_items = 0;
	cmpsc311_cache_max_size = 0;
	cmpsc311_cache_current_items = 0;
	cmpsc311_cache_current_size = 0;
	cmpsc311_cache_initialized = 0;

	// Return successfully
	logMessage(LOG_INFO_LEVEL, "Closed cmpsc311 cache, deleting %d items", dropped);
	return(0);
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : put_cmpsc311_cache
// Description  : Put an object into the cache
//
// Inputs       : cid - cache ID of item to insert
//                buf - the buffer to insert into the cache
//                len - length of buffer inserted into cache
// Outputs      :

int put_cmpsc311_cache(CacheID cid, void *buf, uint32_t len)  {

	/* Cache behavior:
	 *
	 *    you need to set both max_size and max_items objects on initialization
	 *    implement LRU cache, that works as follows:
	 *
	 *      on put operations, keep removing oldest cached entry until there are
	 *      fewer than max_items objects, enough room such that adding the new
	 *      object will result in less or than equal to the max_size number of
	 *      bytes of objects in the cache
	 *
	 *    note: if the object is bigger than the max_size, don't do anything
	 *
	 */

	// Local variables
	Cmpsc311CacheEntry *ref = NULL;
	int ret = 0;

	// Check to see if not initialized
	if (!cmpsc311_cache_initialized) {
		logMessage(LOG_ERROR_LEVEL, "Error, trying to put into uninitialized cache");
		return(-1);
	}

	// Check for reasonable length
	if ((len == 0) || (buf == NULL)) {
		logMessage(LOG_INFO_LEVEL, "put_cmpsc311_cache: ignoring zero length/NULL cache item");
		return(-1);
	}

	// Check to see if it will ever fit (ignore if true)
	if (len > cmpsc311_cache_max_size) {
		logMessage(LOG_INFO_LEVEL, "put_cmpsc311_cache: cache item too large [%u]", len);
		return(-1);
	}

	// Keep ejecting until we have enough room to put the new item in
	while ((cmpsc311_cache_current_items >= cmpsc311_cache_max_items) ||
			(cmpsc311_cache_current_size+len > cmpsc311_cache_max_size)) {
		if  (eject_cmpsc311_cache() == -1) {
			logMessage(LOG_INFO_LEVEL, "put_cmpsc311_cache: unable to eject lines to make space.");
			return(-1);
		}
	}

	// Setup cache line,
	ref = malloc(sizeof(Cmpsc311CacheEntry));
	ref->cid = cid;
	ref->size = len;
	ref->buf = buf;
	gettimeofday(&ref->last_used, NULL);

	// Keep track of cache occupancy, log the new cache state
	cmpsc311_cache_current_items ++;
	cmpsc311_cache_current_size += ref->size;
	logMessage(LOG_INFO_LEVEL, "Cache state [%u items, %u bytes used]", cmpsc311_cache_current_items,
			cmpsc311_cache_current_size);

	// Return the item to the cache, return successfully
	ret = insertValueInHashTable(cmpsc311_htable, cid, ref);
	logMessage(LOG_INFO_LEVEL, "Added cache item OID %u, length %u", cid, len);
	return(ret);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_cmpsc311_cache
// Description  : Get an object from the cache (and return it)
//
// Inputs       : cid - the ID of the cached item to get
// Outputs      : pointer to cached object or NULL if not found

void * get_cmpsc311_cache(CacheID cid) {

	// Local variables
	Cmpsc311CacheEntry *ref;

	// Check to see if not initialized
	if (!cmpsc311_cache_initialized) {
		logMessage(LOG_ERROR_LEVEL, "Error, trying to get from uninitialized cache");
		return(NULL);
	}

	// Return buffer if found, NULL otherwise
	ref = findValueInHashTable(cmpsc311_htable, cid);
	if (ref != NULL) {
		// Reset used field and return the object
		gettimeofday(&ref->last_used, NULL);
		logMessage(LOG_INFO_LEVEL, "Getting found cache item OID %u, length %u", cid, ref->size);
		return(ref->buf);
	}

	// Could not find it, return NULL
	logMessage(LOG_INFO_LEVEL, "Getting cache item OID %u (not found!)", cid);
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : remove_cmpsc311_cache
// Description  : Remove and object from the cache (and return it)
//
// Inputs       : cid - the ID of the object to remove
// Outputs      : pointer to the buffer inserted into the object

void * delete_cmpsc311_cache(CacheID cid) {

	// Local variables
	void *ptr;
	Cmpsc311CacheEntry *ref;

	// Check to see if not initialized
	if (!cmpsc311_cache_initialized) {
		logMessage(LOG_ERROR_LEVEL, "Error, trying to delete from uninitialized cache");
		return(NULL);
	}

	// Find the element in the cache, and return it
	ref = deleteValueFromHashTable(cmpsc311_htable, cid);
	if (ref == NULL) {
		logMessage(LOG_INFO_LEVEL, "Removing (not found) cache item OID %u", cid);
		return(NULL);
	}

	// Keep track of occupancy
	cmpsc311_cache_current_items --;
	cmpsc311_cache_current_size -= ref->size;

	// Cleanup cache line
	logMessage(LOG_INFO_LEVEL, "Removing found cache item OID %u, length %u", cid, ref->size);
	ptr = ref->buf;
	free(ref);

	// log the cache state, return the pointer
	logMessage(LOG_INFO_LEVEL, "Cache state [%u items, %u bytes used]", cmpsc311_cache_current_items,
			cmpsc311_cache_current_size);
	return(ptr);
}

//
// Module local methods

////////////////////////////////////////////////////////////////////////////////
//
// Function     : Eject and item from the cache based on LRU policy
// Description  : eject_cmpsc311_cache
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int eject_cmpsc311_cache(void) {

	// Local variables
	Cmpsc311CacheEntry *ref, *entry;
	HtIterator it;

	// Check to see if not initialized
	if (!cmpsc311_cache_initialized) {
		logMessage(LOG_ERROR_LEVEL, "Error, trying to eject from uninitialized cache");
		return(-1);
	}

	// Walk the hash table
	entry = NULL;
	initHashTableIterator(cmpsc311_htable, &it);
	while ((ref = iterateHashTable(&it)) != NULL) {
		// If no oldest value yet or entry older
		if ((entry == NULL) || (compareTimes(&ref->last_used, &entry->last_used) < 0) ) {
			entry = ref;
		}
	}

	// If nothing to eject, return
	if (entry == NULL) {
		return(-1);
	}

	// Eject and return successfully
	logMessage(LOG_INFO_LEVEL, "Ejecting cache item OID %u, length %u", entry->cid, entry->size);
	deleteValueFromHashTable(cmpsc311_htable, entry->cid);
	cmpsc311_cache_current_items --;
	cmpsc311_cache_current_size -= entry->size;
	logMessage(LOG_INFO_LEVEL, "Cache state [%u items, %u bytes used]", cmpsc311_cache_current_items,
				cmpsc311_cache_current_size);
	free(entry->buf);
	free(entry);
	return(0);
}

//
// Unit test

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311CacheUnitTest
// Description  : Run a UNIT test checking the cache implementation
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int cmpsc311CacheUnitTest(void) {

	// Local variables
	Cmpsc311CacheUnitTestRecord utest_lines[CMPSC311_CACHE_UNIT_TEST_BINS];
	CacheID testOid = CMPSC311_CACHE_UNIT_TEST_STARTVAL;
	uint32_t i, idx, chance;
	void *ptr;

	// Initialize the cache
	if (init_cmpsc311_cache(CMPSC311_CACHE_UNIT_TEST_MAX_ITEMS,CMPSC311_CACHE_UNIT_TEST_MAX_SIZE)) {
		// Fail the unit test on bad initialization
		logMessage(LOG_ERROR_LEVEL, "cmpsc311 cache unit test fail : put fail");
		return(-1);
	}

	// Clear bins (zero them), then iterate until done
	memset(utest_lines, 0x0, sizeof(Cmpsc311CacheUnitTestRecord)*CMPSC311_CACHE_UNIT_TEST_BINS);
	for (i=0; i<CMPSC311_CACHE_UNIT_TEST_ITERATIONS; i++) {

		// Pick a bin to work with, see if it is used
		idx = getRandomValue(0, CMPSC311_CACHE_UNIT_TEST_BINS-1);
		if (utest_lines[idx].buf != NULL) {

			// Now try to get it
			ptr = get_cmpsc311_cache(utest_lines[idx].cid);
			if (ptr == NULL) {

				// Log the fact that we could not find it, mark gone
				logMessage(LOG_INFO_LEVEL, "Could not find OID %u in cache (maybe ejected)",
						utest_lines[idx].cid);
				utest_lines[idx].cid = CMPSC311_NO_CACHE_ID;
				utest_lines[idx].size = 0;
				utest_lines[idx].buf = NULL;

			}  else {

				// Ok, we found it so log the details
				logMessage(LOG_INFO_LEVEL, "Found OID %u in cache (len=%u,ptr=%p)",
						utest_lines[idx].cid, utest_lines[idx].size, utest_lines[idx].buf);
				if (ptr != utest_lines[idx].buf) {
					logMessage(LOG_ERROR_LEVEL, "Found OID %u pointer mismatch, fail (%p!=%p)",
							utest_lines[idx].cid, ptr, utest_lines[idx].buf);
					return(-1);
				}

				// 1 in 5 chance we are going to delete it
				chance = getRandomValue(1,5);
				if (chance == 1) {

					// Delete the item
					if ((ptr=delete_cmpsc311_cache(utest_lines[idx].cid)) != NULL) {
						// Confirm that the pointers match, cleanup buffer
						if (ptr != utest_lines[idx].buf) {
							logMessage(LOG_ERROR_LEVEL, "Delete OID %u pointer mismatch, fail (%p!=%p)",
									utest_lines[idx].cid, ptr, utest_lines[idx].buf);
							return(-1);
						}
						free(utest_lines[idx].buf);
					} else {
						// Log the fact that we could not find it
						logMessage(LOG_INFO_LEVEL, "Could not delete OID %u in cache (maybe ejected)",
								utest_lines[idx].cid);
					}

					// Cleanup the object
					utest_lines[idx].cid = CMPSC311_NO_CACHE_ID;
					utest_lines[idx].size = 0;
					utest_lines[idx].buf = NULL;
				}

			}

		} else {

			// Create a new test line
			utest_lines[idx].cid = testOid++;
			utest_lines[idx].size = getRandomValue(1, CMPSC311_CACHE_UNIT_TEST_MAX_OBJSIZE);
			utest_lines[idx].buf = malloc(utest_lines[idx].size);
			memset(utest_lines[idx].buf, 0x0, utest_lines[idx].size);
			if (put_cmpsc311_cache(utest_lines[idx].cid, utest_lines[idx].buf, utest_lines[idx].size)) {
				// Fail the unit test on bad insert
				logMessage(LOG_ERROR_LEVEL, "cmpsc311 cache unit test fail : put fail");
				return(-1);
			}
			logMessage(LOG_INFO_LEVEL, "Creating new test line OID %u, length %u, ptr=%p",
					utest_lines[idx].cid, utest_lines[idx].size, utest_lines[idx].buf);

		}

	}

	// Close the cache
	logMessage(LOG_INFO_LEVEL, "Closing the cmpsc311 cache.");
	close_cmpsc311_cache();

	// Return successfully
	logMessage(LOG_INFO_LEVEL, "Cache unit test completed successfully.");
	return(0);
}
