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
  char* payload;
  size_t payloadLength;
  size_t payloadMaxLength;
} packet_t;

packet_t* packet_initialize(char* buffer, size_t bufferLength) {
  packet_t* packet = malloc(sizeof(packet_t));
  packet->state = PARTIAL,
  packet->payload = buffer;
  packet->payloadLength = 0;
  packet->payloadMaxLength = bufferLength;
  return packet;
}

const char FRAME_BOUNDARY = '~';
const char ESCAPE = '}';

static ProcessState nextState = SYNC;

void read_packet_payload_from_buffer(circular_buffer_t* buffer, packet_t* packet) {
  for (size_t i = 0; i < buffer->currentSize; i++) {
    char character = circular_buffer_read(buffer);
    switch(nextState) {
      case SYNC:
        if (character == FRAME_BOUNDARY) {
          nextState = QUEUE;
        }
        printf("synchronizing frame, skipping byte %c\n", character);
        break;
      case QUEUE:
        if (character == FRAME_BOUNDARY) {
          nextState = READ;
        } else {
          nextState = SYNC;
        }
        break;
      case READ:
        if (character == FRAME_BOUNDARY) {
          packet->state = COMPLETE;
          nextState = QUEUE;
          return;
        } else if (character == ESCAPE) {
          nextState = READ_ESC;
        } else {
          packet->payload[packet->payloadLength++] = character;
        }
        break;
      case READ_ESC:
        packet->payload[packet->payloadLength++] = character;
        nextState = READ;
        break;
    }
  }
}

void print_packet_payload(packet_t* packet) {
  printf("\nPacket payload: ");
  for (size_t i = 0; i < packet->payloadLength; i++) {
    printf("%c", packet->payload[i]);
  }
  printf("\nPayload length: %zu of %zu\n", packet->payloadLength, packet->payloadMaxLength);
}

#endif
