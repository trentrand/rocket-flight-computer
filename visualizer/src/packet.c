#ifndef _PACKETH_
#define _PACKETH_

#include <string.h>
#include "./circular-buffer.c"

typedef enum {
  SYNC,
  QUEUE,
  READ,
  READ_ESC,
} ProcessState;

typedef enum {
  PARTIAL,
  COMPLETE,
} PacketState;

typedef struct Packet {
  PacketState state;
  uint8_t* payload;
  size_t payloadLength;
  size_t payloadMaxLength;
} packet_t;

packet_t* packet_initialize(uint8_t* buffer, size_t bufferLength) {
  packet_t* packet = malloc(sizeof(packet_t));
  packet->state = PARTIAL,
  packet->payload = buffer;
  packet->payloadLength = 0;
  packet->payloadMaxLength = bufferLength;
  return packet;
}

const uint8_t FRAME_BOUNDARY = '~';
const uint8_t ESCAPE = '}';

static ProcessState nextState = SYNC;

void read_packet_payload_from_buffer(circular_buffer_t* buffer, packet_t* packet) {
  for (size_t i = 0; i < buffer->currentSize; i++) {
    const uint8_t byte = circular_buffer_read(buffer);
    switch(nextState) {
      case SYNC:
        if (byte == FRAME_BOUNDARY) {
          nextState = QUEUE;
        } else {
          printf("%c", byte);
          nextState = SYNC;
        }
        break;
      case QUEUE:
        if (byte == FRAME_BOUNDARY) {
          nextState = READ;
        } else {
          printf("Synchronizing frame\n");
          nextState = SYNC;
        }
        break;
      case READ:
        if (byte == FRAME_BOUNDARY) {
          packet->state = COMPLETE;
          nextState = QUEUE;
          return;
        } else if (byte == ESCAPE) {
          nextState = READ_ESC;
        } else {
          packet->payload[packet->payloadLength++] = byte;
        }
        break;
      case READ_ESC:
        packet->payload[packet->payloadLength++] = byte;
        nextState = READ;
        break;
    }
  }
}

#endif
