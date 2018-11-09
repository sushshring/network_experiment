#ifndef CMPSC311_OPCODE_INCLUDED
#define CMPSC311_OPCODE_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_opcode.h
//  Description   : These are the definitions for the opcode functions of the
//                  cmpsc311 library.  The library processes 64-OP codes for
//                  the library from run-time generated definitions.
//
//                  The definitions generate opcodes where the top bit is
//                  bit zero, the bottom bit is bit 63.
//
//  Author        : Patrick McDaniel
//  Last Modified : Thu Dec 11 12:38:01 EST 2014
//

// Include Files
#include <stdint.h>

// Defines
#define CMPSC311_OPCODE_BITWIDTH 64

//
// Type Definitions

typedef uint64_t OpCode; // This is the code itself for the operator

// The field opcode structure
typedef struct {
	char    *name;      // This is the name of the field
	uint8_t  fieldId;   // This is numeric field identifier
	uint8_t  startBit;  // The bit number that the field starts on
	uint8_t  width;     // The width of the field (in bits)
} CMPSC311FieldDef;

// The opcode structure
typedef struct {
	char             *op_title;         // The name of the opcode being managed
	uint8_t           num_fields;       // This is the number of fields
	uint8_t           assigned_fields;  // The number of fields that have been assigned
	uint8_t           used_bits;        // The number of used bits counting down from 63
	CMPSC311FieldDef *fielddefs;        // These are the field definitions
} Cmpsc311OpcodeDef;

// Opcode library functions

//
// Definition processing

Cmpsc311OpcodeDef * init_opcode_definition(const char *name, uint8_t nofields);
	// allocate an opcode definition structure and initialize

int add_opcode_field(Cmpsc311OpcodeDef *def, const char *name, uint8_t id, uint8_t width);
	// Add a field to the opcode structure

int release_opcode_definition(Cmpsc311OpcodeDef *def);
	// cleanup the opcode definition

char * get_fieldname(Cmpsc311OpcodeDef *def, uint8_t id);
	// Get the name of the field associated with the field id

//
// Log opcode logging routines

int log_opcode_value(Cmpsc311OpcodeDef *def, OpCode op, unsigned long lvl);
	// log the value of the fields/opcodes

int log_opcode_bits(OpCode op, unsigned long lvl);
	// log the bit values of the opcode

//
// Opcode processing

int set_opcode_field(Cmpsc311OpcodeDef *def, uint8_t id, OpCode *op, OpCode *val);
	// Set a field in the opcode

int get_opcode_field(Cmpsc311OpcodeDef *def, uint8_t id, OpCode *op, OpCode *val);
	// Get a field out of the opcode

//
// Unit Tests

int cmpsc311OpCodeUnitTest( void );
  // This is the unit test for the opcode functions.

#endif
