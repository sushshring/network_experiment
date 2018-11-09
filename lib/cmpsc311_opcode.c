////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_opcode.h
//  Description   : These are the functions for the opcode functions of the
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
#include <stdlib.h>
#include <string.h>

// Project Include Files
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <cmpsc311_opcode.h>

// Defines
#define CMPSC311_OPCODE_UNIT_TEST_ITERATIONS 10000
#define CMPSC311_OPCODE_UNIT_TEST_MAX_FIELD_WIDTH 16
#define CMPSC311_OPCODE_UNIT_TEST_MAX_FIELD_ID 0xff
#define CMPSC311_OPCODE_UNIT_TEST_FIELD_ITERATIONS 100
#define CMPSC311_OPCODE_UNIT_TEST_EASY_HIGH_BITS 0xa5a5a5a5
#define CMPSC311_OPCODE_UNIT_TEST_EASY_LOW_BITS 0x5a5a5a5a

// Type definitions
typedef struct OpCodeUintTestStruct {
	uint8_t fieldbits; // The number of bits of fields
	uint8_t fieldid;   // The ID for this field
	OpCode  value;     // The last value set for this field
	char    name[128]; // The name of the field
} OpCodeUintTestVals;

//
// Functional Prototypes
OpCode opcode_shift_in(OpCode in, OpCode val, OpCode startbit, OpCode width);
OpCode opcode_shift_out(OpCode in, OpCode startbit, OpCode width);

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : init_opcode_definition
// Description  : allocate an opcode definition structure and initialize
//
// Inputs       : name - the name of the opcode being defined
//                nofields - the number of fields in the opcode definition
// Outputs      : pointer to definition structure or NULL if failure

Cmpsc311OpcodeDef * init_opcode_definition(const char *name, uint8_t nofields) {

	// Allocate the structure
	Cmpsc311OpcodeDef *def = malloc(sizeof(Cmpsc311OpcodeDef));
	if (def == NULL) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 opcode Allocation failed.");
		return(NULL);
	}

	// Allocate the fields
	def->fielddefs = calloc(nofields,sizeof(CMPSC311FieldDef));
	if (def->fielddefs == NULL) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 opcode Allocation failed.");
		free(def);
		return (NULL);
	}

	// Setup structure and return
	def->op_title = strdup(name);
	def->num_fields = nofields;
	def->assigned_fields = 0;
	def->used_bits = 0;
	return(def);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : add_opcode_field
// Description  : Add a field to the opcode structure
//
// Inputs       : def - definition structure to add field to
//                name - name of the field
//                id - numeric field identifier
//                width - width of the field (in bits)
// Outputs      : 0 if successful, -1 if failure

int add_opcode_field(Cmpsc311OpcodeDef *def, const char *name, uint8_t id, uint8_t width) {

	// Walk the assigned fields to check for duplicates
	int i;
	for (i=0; i<def->assigned_fields; i++) {

		// Check for name duplication
		if (strcmp(name, def->fielddefs[i].name) == 0) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 opcode duplicate field name [%s]", name);
			return(-1);
		}

		// Check for index duplication
		if (def->fielddefs[i].fieldId == id) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 opcode duplicate field ID [%u]", id);
			return(-1);
		}
	}

	// Now check to make sure we can fit it in the structure
	if (def->used_bits + width > CMPSC311_OPCODE_BITWIDTH) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 opcode field extends beyond 64 bits  [%s : %u+%u > %u]",
				name, def->used_bits, width, CMPSC311_OPCODE_BITWIDTH);
		return(-1);
	}

	// Make the assignment
	def->fielddefs[def->assigned_fields].name = strdup(name);
	def->fielddefs[def->assigned_fields].fieldId = id;
	def->fielddefs[def->assigned_fields].startBit = CMPSC311_OPCODE_BITWIDTH - (def->used_bits + width);
	def->fielddefs[def->assigned_fields].width = width;
	def->used_bits += width;
	def->assigned_fields ++;

	// Return successfully
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : release_opcode_definition
// Description  : cleanup the opcode definition
//
// Inputs       : def - definition structure for the opcode
// Outputs      : 0 if successful, -1 if failure

int release_opcode_definition(Cmpsc311OpcodeDef *def) {

	// Cleanup the field definitions
	int i;
	for (i=0; i<def->assigned_fields; i++) {
		free(def->fielddefs[def->assigned_fields].name);
		def->fielddefs[def->assigned_fields].name = NULL;
	}

	// Cleanup the structures
	free(def->op_title);
	free(def->fielddefs);
	free(def);
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_fieldname
// Description  : Get the name of the field associated with the field id
//
// Inputs       : def - definition structure for the opcode
//                id - the field to lookup
// Outputs      : 0 if successful, -1 if failure

char * get_fieldname(Cmpsc311OpcodeDef *def, uint8_t id) {

	// Find the field
	int i = 0;
	while (i < def->assigned_fields) {

		// Is this the correct field?
		if (def->fielddefs[i].fieldId == id) {
			return (def->fielddefs[i].name);
		}

		// Increment to the next field to check
		i++;
	}

	// Return not found
	return (NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : set_opcode_field
// Description  : Set a field in the opcode
//
// Inputs       : def - definition structure for the opcode
//                id - field to define
//                op - opcode to set field in
//                val - value to set in field
// Outputs      : 0 if successful or -1 failure

int set_opcode_field(Cmpsc311OpcodeDef *def, uint8_t id, OpCode *op, OpCode *val) {

	// Local variables
	int i;
	OpCode maxval;

	// Find the field
	i = 0;
	while (i<def->assigned_fields) {

		// Is this the correct field?
		if (def->fielddefs[i].fieldId == id) {

			// Setup the field maxvalue
			maxval = (uint64_t)1 << (uint64_t) def->fielddefs[i].width;
			maxval -= 1;

			// Check to make field passed in fits in the field definition
			if (*val > maxval) {
				logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode set value too large failure [%s: %lu]",
						def->fielddefs[i].name, *val);
				return(-1);
			}

			// Call the elemental shift operation
			*op = opcode_shift_in(*op, *val, def->fielddefs[i].startBit, def->fielddefs[i].width);
			return(0);
		}

		// Increment to next field
		i++;
	}

	// Could not find the field, error out
	logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode set unknown field failure [%u]", id);
	return(-1);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_opcode_field
// Description  : get a field value from the opcode
//
// Inputs       : def - definition structure for the opcode
//                id - field to define
//                op - opcode to get field from
//                val - value to return
// Outputs      : 0 if successful or -1 failure

int get_opcode_field(Cmpsc311OpcodeDef *def, uint8_t id, OpCode *op, OpCode *val) {

	// Find the field
	int i = 0;
	while (i < def->assigned_fields) {

		// Is this the correct field?
		if (def->fielddefs[i].fieldId == id) {
			*val = opcode_shift_out(*op, def->fielddefs[i].startBit, def->fielddefs[i].width);
			return (0);
		}

		// Increment to next field
		i++;
	}

	// Could not find the field, error out
	logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode unknown field failure [%u]", id);
	return (-1);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : opcode_shift_in
// Description  : shift values into the opcode (no error checking)
//
// Inputs       : in - the original opcode
//                val - the value to shift in
//                startbit - the starting bit (0 is LSB)
// Outputs      : 0 if successful or -1 failure

OpCode opcode_shift_in(OpCode in, OpCode val, OpCode startbit, OpCode width) {

	// Local variables
	OpCode mask, imask, out;

	// First generate a mask of the field, and inverted mask
	mask = ((OpCode)1 << width) - 1; // All ones on the field
	mask = mask << startbit;
	imask = ~mask;

	// Mask out the value and output value
	val = ((val<<startbit) & mask);
	out = (in & imask);

	// Last OR them together
	out = (out | val);

	// Return the final value
	return(out);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : opcode_shift_out
// Description  : shift values into the opcode (no error checking)
//
// Inputs       : in - the original opcode
//                val - the value to shift in
//                startbit - the starting bit (0 is LSB)
// Outputs      : 0 if successful or -1 failure

OpCode opcode_shift_out(OpCode in, OpCode startbit, OpCode width) {

	// Local variables
	OpCode mask, out;

	// Generate a mask of the field, then mask out the shifted value
	mask = ((OpCode)1 << width) - (OpCode)1; // All ones on the field
	out = in >> startbit;

	// Last AND them together
	out = (out & mask);

	// Return the final value
	return(out);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : log_opcode_value
// Description  : log the value of the fields/opcodes
//
// Inputs       : def - the opcode definition structure
//                op - the opcode value to print
//                lvl - the log level of the value
// Outputs      : 0 if successful or -1 failure

int log_opcode_value(Cmpsc311OpcodeDef *def, OpCode op, unsigned long lvl) {

	// Local variables
	int i;
	OpCode val;

	// If the level not set, bail out
	if (!levelEnabled(lvl)) {
		return(0);
	}

	// Print out header, walk fields
	logMessage(lvl, "OPCODE VALUE %s {", def->op_title);
	for (i=0; i<def->num_fields; i++) {

		// Get each field
		if(get_opcode_field(def, i, &op, &val)) {
			logMessage(lvl, "  %20s = [BAD VALUE]", get_fieldname(def, i));
		} else {
			logMessage(lvl, "  %20s = %lx", get_fieldname(def, i), val);
		}
	}
	logMessage(lvl, "};");

	// Return successfully
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : log_opcode_bits
// Description  : log the bit values of the opcode
//
// Inputs       : def - the opcode definition structure
//                op - the opcode value to print
//                lvl - the log level of the value
// Outputs      : 0 if successful or -1 failure

int log_opcode_bits(OpCode op, unsigned long lvl) {

	// Local variables
	int i;
	char str[70];

	// If the level not set, bail out
	if (!levelEnabled(lvl)) {
		return(0);
	}

	// Setup the binary string
	memset(str, 0x0, 70);
	for (i=0; i<64; i++) {
		str[i] = ((op >> (63-i)) & 1) ? '1' : '0';
	}

	// Print the header information
	logMessage(lvl, "   6         5         4         3         2         1         0");
	logMessage(lvl, "3210987654321098765432109876543210987654321098765432109876543210");
	logMessage(lvl, str);

	// Return successfully
	return(0);
}

//
// Unit test code

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cmpsc311OpCodeUnitTest
// Description  : This is the unit test for the opcode functions.
//
// Inputs       : none
// Outputs      : 0 if successful or -1 failure

int cmpsc311OpCodeUnitTest( void ) {
	// Local variables
	int i, j, fields, bits, foundid, field;
	OpCodeUintTestVals vals[64];
	Cmpsc311OpcodeDef *def;
	OpCode op, val;

	// ELEMENTAL TEST
	// Lets do a test of the low level functionality and confirm that it works correctly (shift in)
	if ((opcode_shift_in(0,0xffff,0,16) != 0xffff) ||
			(opcode_shift_in(0,0xffff,16,16) != 0xffff0000) ||
			(opcode_shift_in(0,0xffff,32,16) != 0xffff00000000) ||
			(opcode_shift_in(0,0xffff,48,16) != 0xffff000000000000)) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest elemental shift in test failed.");
		return(-1);
	}

	// Lets do a test of the low level functionality and confirm that it works correctly (shift in)
	if ((opcode_shift_out(0xffff,0,16) != 0xffff) ||
			(opcode_shift_out(0xffff0000,16,16) != 0xffff) ||
			(opcode_shift_out(0xffff00000000,32,16) != 0xffff) ||
			(opcode_shift_out(0xffff000000000000,48,16) != 0xffff)) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest elemental shift out test failed.");
		return(-1);
	}

	// EASY TEST
	// Lets do a pre-test of an easy version
	// This is a definition of two 32-bit fields
	if (((def = init_opcode_definition("Unit test (easy)", 2)) == NULL) ||
			(add_opcode_field(def, "field 1", 0, 32)) ||
			(add_opcode_field(def, "field 2", 1, 32)) )	{
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy definition create.");
		return(-1);
	}

	// Stage one, get zeros
	logMessage(LOG_INFO_LEVEL, "CMPSC311 opcode utest easy stage one");
	op = 0;
	if ((get_opcode_field(def, 0, &op, &val)) || (val != 0) ) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy first zero get.");
		return(-1);
	}
	if (val != 0) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy get first zero mismatch.");
		return(-1);
	}
	if ((get_opcode_field(def, 1, &op, &val)) || (val != 0) ) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy second zero get.");
		return(-1);
	}
	if (val != 0) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy get second zero mismatch.");
		return(-1);
	}
	logMessage(LOG_INFO_LEVEL, "CMPSC311 opcode utest easy stage one complete.");


	// ** Stage two, set values
	logMessage(LOG_INFO_LEVEL, "CMPSC311 opcode utest easy stage two");
	val = CMPSC311_OPCODE_UNIT_TEST_EASY_HIGH_BITS;
	if (set_opcode_field(def, 0, &op, &val)) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy first nonzero set.");
		return(-1);
	}
	val = CMPSC311_OPCODE_UNIT_TEST_EASY_LOW_BITS;
	if (set_opcode_field(def, 1, &op, &val)) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy second nonzero set.");
		return(-1);
	}
	logMessage(LOG_INFO_LEVEL, "CMPSC311 opcode utest easy stage two complete.");


	// *** Stage three, get non-zeros
	logMessage(LOG_INFO_LEVEL, "CMPSC311 opcode utest easy stage three");
	if (get_opcode_field(def, 0, &op, &val) ) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy first zero get.");
		return(-1);
	}
	if (val != CMPSC311_OPCODE_UNIT_TEST_EASY_HIGH_BITS) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy get first non-zero mismatch.");
		return(-1);
	}
	if (get_opcode_field(def, 1, &op, &val)) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy second zero get.");
		return(-1);
	}
	if (val != CMPSC311_OPCODE_UNIT_TEST_EASY_LOW_BITS) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on easy get second non-zero mismatch.");
		return(-1);
	}
	logMessage(LOG_INFO_LEVEL, "CMPSC311 opcode utest easy stage three complete.");

	// Just try some bit code printing
	log_opcode_value(def, op, LOG_INFO_LEVEL);
	log_opcode_bits(op, LOG_INFO_LEVEL);

	// Cleanup the definition
	if (release_opcode_definition(def)) {
		logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on definition release.");
		return(-1);
	}

	// NOW DO THE MORE ELABORATE TEST
	// Iterate a specific number of times
	for (i=0; i<CMPSC311_OPCODE_UNIT_TEST_ITERATIONS; i++) {

		// Now make up some definitions
		bits = 0;
		fields = 0;
		while (bits<64) {

			// First make the number of bits for new field definition
			vals[fields].fieldbits = getRandomValue(1, CMPSC311_OPCODE_UNIT_TEST_MAX_FIELD_WIDTH);
			if (bits + vals[fields].fieldbits > 64) {
				vals[fields].fieldbits = 64-bits;
			}
			bits += vals[fields].fieldbits;

			// Now make an ID for the field
			foundid = 0;
			while (!foundid) {
				// Create Id, look for it
				vals[fields].fieldid = getRandomValue(1, CMPSC311_OPCODE_UNIT_TEST_MAX_FIELD_ID);
				foundid = 1;
				for (j=0; j<fields; j++) {
					if (vals[fields].fieldid == vals[j].fieldid) {
						foundid = 0;
					}
				}
			}

			// Zero value, mark the new field as added
			vals[fields].value = 0;
			fields++;
		}

		// Create the definition
		if ((def = init_opcode_definition("utest", fields)) == NULL) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on definition create.");
			return(-1);
		}
		logMessage(LOG_INFO_LEVEL, "CMPSC311 Opcode definition create  [%s]", def->op_title);

		// Now create the fields
		for (j=0; j<fields; j++) {
			// Setup name and create field
			snprintf(vals[j].name, 128, "OPCODE FIELD %u ID=%u width=%u", j, vals[j].fieldid, vals[j].fieldbits);
			if (add_opcode_field(def, vals[j].name, vals[j].fieldid, vals[j].fieldbits)) {
				logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on field create [%s]",
						vals[j].name);
				return(-1);
			}
			logMessage(LOG_INFO_LEVEL, "CMPSC311 Opcode Unit create field [%s]", vals[j].name);
		}

		// Now do a bunch of assignments
		op = 0;
		for (j=0; j<CMPSC311_OPCODE_UNIT_TEST_FIELD_ITERATIONS; j++) {

			// Pick a field
			field = getRandomValue(0, fields-1);

			// Pick an operation (set or get)
			if (getRandomValue(0,1)) {

				// Pick and set a value, check to make sure it worked
				vals[field].value = getRandomValue(0, (1<<vals[field].fieldbits)-1);
				if (set_opcode_field(def, vals[field].fieldid, &op, &vals[field].value)) {
					logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on field set [%s:%u/%u]",
							vals[field].name, vals[field].fieldid, vals[field].value);
					return(-1);
				}

				// Log successful set
				logMessage(LOG_INFO_LEVEL, "CMPSC311 Opcode utest field set [field[%u] = %lu]",
						field, vals[field].value);

			} else {

				// Set a value, check to make sure it worked
				if (get_opcode_field(def, vals[field].fieldid, &op, &val)) {
					logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on field get [%s:%u/%u]",
							vals[field].name, vals[field].fieldid, vals[field].value);
					return(-1);
				}

				// Now make sure the value is what it should be
				if (vals[field].value != val) {
					logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on field mismatch [%s:%u/%u!=%u]",
							vals[field].name, vals[field].fieldid, vals[field].value, val);
					return(-1);
				}

				// Log successful get
				logMessage(LOG_INFO_LEVEL, "CMPSC311 Opcode utest field get confirm [field[%u] == %lu]",
						vals[field].fieldid, vals[field].value);

			}

		}

		// Cleanup the definition
		if (release_opcode_definition(def)) {
			logMessage(LOG_ERROR_LEVEL, "CMPSC311 Opcode utest failed on definition release.");
			return(-1);
		}

	}

	// Log and return successfully
	logMessage(LOG_INFO_LEVEL, "Opcode utest completed successfully.");
	return(0);
}
