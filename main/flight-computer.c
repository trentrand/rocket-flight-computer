#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/gpio.h>
#include <driver/uart.h>

#include "./time.c"
#include "./flash-storage.c"
#include "./network.c"
#include "./imu.c"
#include "bno055.h"

#include "./models/telemetry.pb-c.h"

const char* TAG = "Flight Computer";

const uint8_t boundaryFlag = 0x7E;
const uint8_t escapeFlag = 0x7D;

size_t pack_payload_to_frame(const uint8_t *data, size_t dataLength, uint8_t *outputBuffer) {
  size_t writeHeadIndex = 0;
  outputBuffer[writeHeadIndex++] = boundaryFlag;
  for (size_t readHeadIndex = 0; readHeadIndex < dataLength; readHeadIndex++) {
    if (*(data + readHeadIndex) == boundaryFlag || *(data + readHeadIndex) == escapeFlag) {
      outputBuffer[writeHeadIndex++] = escapeFlag;
      outputBuffer[writeHeadIndex++] = *(data + readHeadIndex);
    } else {
      outputBuffer[writeHeadIndex++] = *(data + readHeadIndex);
    }
  }
  outputBuffer[writeHeadIndex++] = boundaryFlag;
  return writeHeadIndex;
}

void app_main() {
  initialize_flash_storage();
  wifi_connect();
  synchronize_system_clock();

  initialize_bno055();

  // Configure UART
  const int uart_num = UART_NUM_0;
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };

  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  ESP_ERROR_CHECK(uart_driver_install(uart_num, 128 * 2, 0, 0, NULL, 0));

  // Initialize Telemetry proto buffer
  Telemetry telemetry = TELEMETRY__INIT;

  // Transmit telemetry data
  uint8_t *buffer;
  uint8_t *serialOutputBuffer;


  while (1) {
    bno055_quaternion_t quaternion;
    bno055_get_quaternion(0, &quaternion);

    int64_t now = timestamp_microseconds();
    telemetry.timestampstart = now;
    telemetry.x = quaternion.x;
    telemetry.y = quaternion.y;
    telemetry.z = quaternion.z;

    int bufferLength = telemetry__get_packed_size(&telemetry);
    buffer = (uint8_t*) heap_caps_calloc(1, bufferLength, MALLOC_CAP_8BIT);
    serialOutputBuffer = (uint8_t*) heap_caps_calloc(1, bufferLength, MALLOC_CAP_8BIT);

    telemetry__pack(&telemetry, buffer);

    const size_t serialOutputBufferLength = pack_payload_to_frame(buffer, bufferLength, serialOutputBuffer);
    uart_write_bytes(uart_num, (const char*)serialOutputBuffer, serialOutputBufferLength);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  free(buffer);
}
