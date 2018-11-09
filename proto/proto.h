//
// Created by sps5394 on 11/7/18.
//

#ifndef CLIENT_PROTO_H
#define CLIENT_PROTO_H
#define MAX_BLOCK_SIZE 8096
#ifndef __linux__
// CLION IMPORTS
#include <strings.h>
#endif

#include <stdlib.h>
#include <string.h>

typedef enum {
  REQ_FILE,
  REQ_ACK,
  FILE_BLOCK,
  FILE_BLOCK_ACK,
  FILE_COMPLETE,
  FILE_COMPLETE_ACK,
} PROTO_TYPE;


typedef struct {
  PROTO_TYPE msgType;
  char data[MAX_BLOCK_SIZE];
} ProtoMsg;

////////////////////////////////////////////////////////////////////////////////
//
// Function     : marshall
// Description  : Converts the ProtoMsg struct into a marshalled buffer
//
// Inputs       : msg: ProtoMsg object to marshall
//                buffer: buffer to write data to
// Outputs      : 0 if successful, -1 if failure
int marshall(ProtoMsg *msg, unsigned char *buffer);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : marshall
// Description  : Converts a marshalled buffer into a ProtoMsg struct instance
//
// Inputs       : msg: ProtoMsg object to unmarshall to
//                buffer: buffer to read data from
// Outputs      : 0 if successful, -1 if failure
int unmarshall(unsigned char *buffer, ProtoMsg *msg);

#endif //CLIENT_PROTO_H
