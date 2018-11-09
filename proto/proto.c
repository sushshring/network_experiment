//
// Created by sps5394 on 11/7/18.
//

#include "proto.h"

int marshall(ProtoMsg *msg, unsigned char *buffer) {
  if (buffer == NULL) {
    return -1;
  }
  memcpy(buffer, &msg->msgType, sizeof(PROTO_TYPE));
  memcpy(buffer + sizeof(PROTO_TYPE), msg->data, MAX_BLOCK_SIZE);
  return 0;
}

int unmarshall(unsigned char *buffer, ProtoMsg *msg) {
  memcpy(&msg->msgType, buffer, sizeof(PROTO_TYPE));
  memcpy(msg->data, buffer + sizeof(PROTO_TYPE), MAX_BLOCK_SIZE);
  return 0;
}

