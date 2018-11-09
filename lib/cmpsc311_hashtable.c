////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_hashtable.h
//  Description   : This is a generic hashtable implementation used for
//                  data structure storage and access.
////
//  Author        : Patrick McDaniel
//  Created       : Sat Sep  6 08:56:10 EDT 2014
//

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cmpsc311_hashtable.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

// Defines
#define HASH_VALUE(x,h) (x%h->htTableSize)
#define VALIDATE_HT_ENTRY(x) CMPSC_ASSERT0((x->cookie == HT_COOKIE_VALUE), \
		"Cookie corruption: memory error likely");
#define HT_UNIT_TEST_BITSIZE 8
#define HT_UNIT_TEST_BLK_SIZE 16

//
// Hash Table Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : initHashtable
// Description  : This function initializes the hash table to a width
//                of 2^(bits) width
//
// Inputs       : ht - the hash table to initialize
//                bits - the number of bits in the number
// Outputs      : 0 if successful test, -1 if failure

int initHashTable( HTable *ht, uint16_t bits ) {

	// Setup the hash table to empty state
	assert(bits<16);
	ht->htTableSize = 1<<bits;
	ht->elements = 0;
	ht->hasHTable = malloc(sizeof(HtEntryData *)*ht->htTableSize);
	memset( ht->hasHTable, 0x0, sizeof(HtEntryData *)*ht->htTableSize );

	// Return successfully
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cleanupHashtable
// Description  : Cleanup the hash table, delete all structures
//
// Inputs       : ht - the hash table to clean up
// Outputs      : 0 if successful test, -1 if failure

int cleanupHashTable( HTable *ht ) {

	// Local variables
	int i;
	HtEntryData *cur, *sav;

	// Walk the linked lists, cleaning up the entries
	for (i=0; i<ht->htTableSize; i++) {
		cur = ht->hasHTable[i];
		while ( cur != NULL ) {
			VALIDATE_HT_ENTRY(cur);
			sav = cur;
			cur = cur->next;
			free( sav->block );
			free( sav );
		}
		ht->hasHTable[i] = NULL;
	}

	// Finally, cleanup the entry table and return
	free( ht->hasHTable );
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : insertValueInHashTable
// Description  : Insert a value into the hash table of value idx, block blk
//
// Inputs       : ht - the hash table to insert data into
//                idx - the index value of the thing to insert
//                the - the block to insert
// Outputs      : 0 if successful test, -1 if failure

int insertValueInHashTable( HTable *ht, HtIndexValue idx, void *blk ) {

	// Local variables
	uint16_t htIdx;
	HtEntryData *cur, *last, *entry;

	// Sanity check insert (look for duplicate assert)
	CMPSC_ASSERT1((findValueInHashTable(ht, idx) == NULL),
			"Duplicate index value %d inserted into hash table", idx); ;

	// Setup the new structure
	entry = malloc(sizeof(HtEntryData));
	entry->cookie = HT_COOKIE_VALUE;
	entry->index = idx;
	entry->block = blk;
	entry->prev = NULL;
	entry->next = NULL;
	VALIDATE_HT_ENTRY(entry);

	// Compute the index
	htIdx = HASH_VALUE(idx, ht);
	cur = ht->hasHTable[htIdx];
	last = NULL;

	// Check for empty bin or want to insert at head of list
	if ( cur == NULL ) {
		ht->hasHTable[htIdx] = entry;
		ht->elements ++;
		return( 0 );
	}

	// check to add to the head of the list
	VALIDATE_HT_ENTRY(cur);
	if ( cur->index >= idx ) {
		ht->hasHTable[htIdx] = entry;
		entry->next = cur;
		cur->prev = entry;
		ht->elements ++;
		return( 0 );
	}

	// Walk the list looking for insert point
	do  {
		last = cur;
		cur = cur->next;
	} while ( (cur != NULL) && (cur->index < idx) );

	// Ok, found place to insert
	entry->prev = last;
	entry->next = cur;
	last->next = entry;
	if ( cur != NULL ) {
		VALIDATE_HT_ENTRY(cur);
		cur->prev = entry;
	}
	ht->elements ++;

	// Return successfully
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : findValueInHashTable
// Description  : Find a block for a particular index value in the table
//
// Inputs       : idx - the index value of the thing to find
// Outputs      : pointer to block item if found, NULL otherwise

void * findValueInHashTable( HTable *ht, HtIndexValue idx ) {

	// Walk the structure for that hash table value
	HtEntryData *cur = ht->hasHTable[HASH_VALUE(idx, ht)];
	while ((cur != NULL) && (cur->index <= idx) ) {
		// Is this the right one?
		VALIDATE_HT_ENTRY(cur);
		if ( cur->index == idx ) {
			return( cur->block );
		}
		cur = cur->next;
	}

	// Return not found
	return( NULL );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : deleteValueFromHashTable
// Description  : Delete a value from the hashtable of value idx, return it
//
// Inputs       : idx - the index value of the thing to delete
// Outputs      : pointer to block if found, NULL if not found

void * deleteValueFromHashTable( HTable *ht, HtIndexValue idx ) {

	// Walk the structure for that hash table value
	uint16_t tblidx = HASH_VALUE(idx, ht);
	HtEntryData *cur = ht->hasHTable[tblidx];
	void *blk;

	// See if the head of the list is
	while ( cur != NULL ) {

		// Is this the right one?
		VALIDATE_HT_ENTRY(cur);
		if ( cur->index == idx ) {

			// If front of list, then reset the front of the list
			if ( cur == ht->hasHTable[tblidx] ) {
				 ht->hasHTable[tblidx] = cur->next;
			}

			// If the previous element exists, reset its "next" pointer
			if ( cur->prev != NULL ) {
				cur->prev->next = cur->next;
			}

			// If the next element exists, reset its "previous" pointer
			if ( cur->next != NULL ) {
				cur->next->prev = cur->prev;
			}

			// Cleanup the structure and return the block
			blk = cur->block;
			free( cur );
			ht->elements --;
			return( blk );
		}

		// Move to the next entry
		cur = cur->next;
	}

	// Return not found
	return( NULL );
}

//
// Iterator Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : initHashTableIterator
// Description  : Initialize the iterator structure
//
// Inputs       : ht - the hash table to iterate on
//                it - the iterator object
// Outputs      : 0 if successful test, -1 if failure

int initHashTableIterator( HTable *ht, HtIterator *it ) {

	// Zero up the iterator structure
	assert( it != NULL );
	it->table = ht;
	it->idx = 0;
	it->ptr = NULL;
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : iterateHashTable
// Description  : iterate through the hash table, returns the next value
//                in the table
//
// Inputs       : it - the iterator object
// Outputs      : 0 if successful test, -1 if failure

void * iterateHashTable( HtIterator *it ) {

	// If we are on a live linked list
	if ( it->ptr != NULL ) {
		// Walk the pointer, return if still non-NULL
		it->ptr = it->ptr->next;
		if ( it->ptr != NULL ) {
			return( it->ptr->block );
		}

		// Ok, then walk to the next linked list
		it->idx ++;
	}

	// Find the first nonempty list
	while ( it->idx < it->table->htTableSize ) {
		if ( it->table->hasHTable[it->idx] != NULL ) {
			it->ptr = it->table->hasHTable[it->idx];
			return( it->ptr->block );
		}
		it->idx ++;
	}

	// Reached the end of the list, return NULL
	return( NULL );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hasHTableUnitTest
// Description  : Perform a test of the hash table functionality
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int hashTableUnitTest( void ) {

	// Local variables
	uint16_t i, bitmax,*values = NULL, tmpidx, tmp, *blkval;
	HTable *ht = NULL;
	HtIterator iter;

	// Setup some source numbers in a random permutation
	bitmax = (1<<HT_UNIT_TEST_BITSIZE)*64;
	values = malloc(sizeof(uint16_t)*bitmax);
	for (i=0; i<bitmax; i++) {
		values[i] = i;
	}

	// Now randomly permute the values
	for (i=0; i<bitmax; i++) {
		tmpidx = getRandomValue(0, bitmax-1);
		tmp = values[tmpidx];
		values[tmpidx] = values[i];
		values[i] = tmp;
	}

	// First create a hash table of some fixed sized
	ht = malloc(sizeof(HTable));
	initHashTable( ht, HT_UNIT_TEST_BITSIZE );

	// Insert the values by setting up block, then insert
	for (i=0; i<bitmax; i++) {
		blkval = malloc(sizeof(uint16_t));
		*blkval = values[i];
		insertValueInHashTable(ht, values[i], blkval);
		logMessage(LOG_OUTPUT_LEVEL, "HT Unit Test: inserted %3u/%3d [%p]",
				*blkval, values[i], blkval);
	}

	// Walk and print the hash table contents
	initHashTableIterator(ht, &iter);
	while ( (blkval = iterateHashTable(&iter)) != NULL ) {
		logMessage(LOG_OUTPUT_LEVEL, "HT Unit Test: iterator %3u [%p]", *blkval, blkval);
	}

	// Now attempt to find all of the values
	for (i=0; i<bitmax; i++) {

		// Find the value in the hash table
		if ( (blkval = findValueInHashTable(ht, i)) == NULL ) {
			logMessage( LOG_ERROR_LEVEL, "HT Unit Test: Failure finding hashtable entry %d", i );
			return( -1 );
		}

		// Now check to see if the number if correct
		if ( *blkval != i ) {
			logMessage( LOG_ERROR_LEVEL,
					"HT Unit Test: Incorrect find in hashtable, entry %d (%d)", i, *blkval );
			return( -1 );
		}
	}

	// Now delete all of the items from the table
	for (i=0; i<bitmax; i++) {

		// Delete and check result
		if ( (blkval = deleteValueFromHashTable(ht, values[i])) == NULL ) {
			logMessage( LOG_ERROR_LEVEL,
					"HT Unit Test: Failure deleting hashtable entry %d [idx %d]", i, values[i] );
			return( -1 );
		}

		// Now check to see if the number if correct
		if ( *blkval != values[i] ) {
			logMessage( LOG_ERROR_LEVEL, "HT Unit Test: Incorrect delete in hashtable, entry %d", i );
			return( -1 );
		}

		// Print ouf that we go the value
		logMessage(LOG_OUTPUT_LEVEL, "HT Unit Test: delete %d %3u/%3d [%p]",
						i, *blkval, values[i], blkval);
		free( blkval );
	}

	// Now cleanup the hash table, return successfully
	cleanupHashTable( ht );
	return( 0 );
}
