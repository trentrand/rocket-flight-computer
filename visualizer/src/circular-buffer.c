#ifndef _CIRCULARBUFFERH_
#define _CIRCULARBUFFERH_

#include <assert.h>
#include <stdbool.h>

typedef struct circular_buffer {
  unsigned int tail;
  unsigned int head;
  size_t currentSize;
  size_t length;
  char* data;

} circular_buffer_t;

circular_buffer_t* circular_buffer_initialize(size_t length) {
  circular_buffer_t* buffer = malloc(sizeof(circular_buffer_t));
  buffer->tail = -1;
  buffer->head = 0;
  buffer->currentSize = 0;
  buffer->length = length;
  buffer->data = (char*) calloc(length, sizeof(char));
  return buffer;
}

bool circular_buffer_is_full(circular_buffer_t* buffer) {
  return buffer->currentSize == buffer->length;
}

bool circular_buffer_is_empty(circular_buffer_t* buffer) {
  return buffer->currentSize == 0;
}

int circular_buffer_write(circular_buffer_t* buffer, char* inputData, size_t inputLength) {
  assert(buffer != NULL);
  if (circular_buffer_is_full(buffer)) {
    return 0;
  }
  for (size_t inputDataWriteIndex = 0; inputDataWriteIndex < inputLength; inputDataWriteIndex++) {
    buffer->tail = (buffer->tail + 1) % buffer->length;
    buffer->data[buffer->tail] = *(inputData + inputDataWriteIndex);
  }
  buffer->currentSize += inputLength;
  printf("buffer size: %zu\n", buffer->currentSize);
  return inputLength;
}

char circular_buffer_read(circular_buffer_t* buffer) {
  assert(buffer != NULL);
  assert(!circular_buffer_is_empty(buffer));

  char outputData = buffer->data[buffer->head];
  buffer->head = (buffer->head + 1) % buffer->length;
  buffer->currentSize -= 1;

  return outputData;
}

void circular_buffer_free(circular_buffer_t* buffer) {
  free(buffer->data);
  free(buffer);
}

#endif
