#include <libserialport.h>
#include <string.h>

struct sp_port *port;

unsigned int timeout = (8 * sizeof(char)) / 115200;

// Helper function for error handling
int check(enum sp_return result) {
  char *error_message;

  switch (result) {
    case SP_ERR_ARG:
      printf("Error: Invalid argument.\n");
      abort();
    case SP_ERR_FAIL:
      error_message = sp_last_error_message();
      printf("Error: Failed: %s\n", error_message);
      sp_free_error_message(error_message);
      abort();
    case SP_ERR_SUPP:
      printf("Error: Not supported.\n");
      abort();
    case SP_ERR_MEM:
      printf("Error: Couldn't allocate memory.\n");
      abort();
    case SP_OK:
    default:
      return result;
  }
}

void serial_initialize(char *port_name) {
  printf("Opening serial port '%s'\n", port_name);
  check(sp_get_port_by_name(port_name, &port));
  check(sp_open(port, SP_MODE_READ_WRITE));

  // TODO: we can construct this dynamically with sp_get_config()
  printf("Configuring port to 115200 8N1, no flow control.\n");
  check(sp_set_baudrate(port, 115200));
  check(sp_set_bits(port, 8));
  check(sp_set_parity(port, SP_PARITY_NONE));
  check(sp_set_stopbits(port, 1));
  check(sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE));
}

int serial_read(char* buffer, int bufferLength) {
  int numberOfBytesReadBeforeTimeout = check(sp_blocking_read(port, buffer, bufferLength, timeout));

  if (numberOfBytesReadBeforeTimeout != bufferLength) {
    printf("Timed out, %d/%d bytes received\n", numberOfBytesReadBeforeTimeout, bufferLength);
  }
  return numberOfBytesReadBeforeTimeout;
}

int serial_read_nonblocking(char* buffer, int bufferLength) {
  printf("Receiving %d bytes on port %s\n", bufferLength, sp_get_port_name(port));
  int bytesWaitingForRead = sp_input_waiting(port);
  if (bytesWaitingForRead > 0) {
    printf("%i bytes waiting for read\n", bytesWaitingForRead);
    return sp_nonblocking_read(port, buffer, bufferLength);
  }
  return -1;
}

void serial_close() {
  check(sp_close(port));
  sp_free_port(port);
}
