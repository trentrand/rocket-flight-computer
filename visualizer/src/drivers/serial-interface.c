#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int serialPortDescriptor;

void set_serial_interface_attributes(int serialDescriptor, int speed, int parity) {
  struct termios tty;
  if (tcgetattr(serialDescriptor, &tty) != 0) {
    printf("error getting attributes from serial interface");
    return;
  }

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
  // disable IGNBRK for mismatched speed tests, otherwise receive break as \000 chars
  tty.c_iflag &= ~IGNBRK; // disable break processing
  tty.c_lflag = 0; // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0; // no remapping, no delays
  tty.c_cc[VMIN] = 0; // read doesn't block
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls,
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(serialDescriptor, TCSANOW, &tty) != 0) {
    printf("error setting serial interface attributes with speed %d and parity %d\n", speed, parity);
    return;
  }
}

void set_serial_blocking(int serialDescriptor, int should_block) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(serialDescriptor, &tty) != 0) {
    printf("error getting attributes from serial interface");
    return;
  }

  tty.c_cc[VMIN] = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;

  if (tcsetattr(serialDescriptor, TCSANOW, &tty) != 0) {
    printf("error setting serial interface blocking attribute to %s\n", should_block == 1 ? "enabled" : "disabled");
  }
}

void setup_serial_stream(char* serialPortIdentifier) {
  serialPortDescriptor = open(serialPortIdentifier, O_RDWR | O_NOCTTY | O_SYNC);
  if (serialPortDescriptor < 0) {
    printf("Error opening serial port: %d\n", serialPortDescriptor);
    return;
  }
  set_serial_interface_attributes(serialPortDescriptor, B115200, 0);
  set_serial_blocking(serialPortDescriptor, 1);
}

int read_serial_stream(uint8_t* buffer, int bufferLength) {
  usleep(bufferLength * 100);
  memset(buffer, '\0', bufferLength);
  int numberOfBytesRead = read(serialPortDescriptor, buffer, bufferLength);
  return numberOfBytesRead;
}

void close_serial_stream() {
  close(serialPortDescriptor);
}
