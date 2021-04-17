#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <driver/gpio.h>
#include <driver/uart.h>

/* #include "BNO055ESP32.h" */
#include "./models/telemetry.pb-c.h"

#include "./time.c"
#include "./flash-storage.c"
#include "./network.c"

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

  // TODO: which UART do we dedicate to BNO055? Note: UART_NUM_0 is for usb telemetry stream
  /* BNO055 bno(UART_NUM_0, (gpio_num_t)CONFIG_BNO055_SCL_GPIO, (gpio_num_t)CONFIG_BNO055_SDA_GPIO); */

  /* try { */
  /*   bno.begin(); */
  /*   bno.enableExternalCrystal(); */
  /*   bno.setOprModeNdof(); */
  /*   ESP_LOGI(TAG, "Setup Done."); */
  /* } catch (BNO055BaseException& ex) { */
  /*   ESP_LOGE(TAG, "Setup Failed, Error: %s", ex.what()); */
  /*   return; */
  /* } catch (std::exception& ex) { */
  /*   ESP_LOGE(TAG, "Setup Failed, Error: %s", ex.what()); */
  /*   return; */
  /* } */

  // Initialize Telemetry proto buffer
  Telemetry telemetry = TELEMETRY__INIT;

  // Transmit telemetry data
  uint8_t *buffer;
  uint8_t *serialOutputBuffer;

  while (1) {
    int64_t now = timestamp_microseconds();
    telemetry.timestampstart = now;

    int bufferLength = telemetry__get_packed_size(&telemetry);
    buffer = (uint8_t*) heap_caps_calloc(1, bufferLength, MALLOC_CAP_8BIT);
    serialOutputBuffer = (uint8_t*) heap_caps_calloc(1, bufferLength, MALLOC_CAP_8BIT);

    telemetry__pack(&telemetry, buffer);

    /* try { */
    /* int8_t temperature = bno.getTemp(); */
    /* ESP_LOGI(TAG, "TEMP: %d°C", temperature); */

    /* int16_t sw = bno.getSWRevision(); */
    /* uint8_t bl_rev = bno.getBootloaderRevision(); */
    /* ESP_LOGI(TAG, "SW rev: %d, bootloader rev: %u", sw, bl_rev); */

    /* bno055_self_test_result_t res = bno.getSelfTestResult(); */
    /* ESP_LOGI(TAG, "Self-Test Results: MCU: %u, GYR:%u, MAG:%u, ACC: %u", res.mcuState, res.gyrState, res.magState, */
    /*     res.accState); */
    /* } catch (BNO055BaseException& ex) { */
    /* ESP_LOGE(TAG, "Something bad happened: %s", ex.what()); */
    /* return; */
    /* } catch (std::exception& ex) { */
    /* ESP_LOGE(TAG, "Something bad happened: %s", ex.what()); */
    /* return; */
    /* } */

    /* try { */
    /*   bno055_calibration_t cal = bno.getCalibration(); */
    /*   bno055_vector_t v = bno.getVectorEuler(); */
    /*   ESP_LOGI(TAG, "Euler: X: %.1f Y: %.1f Z: %.1f || Calibration SYS: %u GYRO: %u ACC:%u MAG:%u", v.x, v.y, v.z, cal.sys, */
    /*       cal.gyro, cal.accel, cal.mag); */
    /* } catch (BNO055BaseException& ex) { */
    /*   ESP_LOGE(TAG, "Something bad happened: %s", ex.what()); */
    /*   return; */
    /* } catch (std::exception& ex) { */
    /*   ESP_LOGE(TAG, "Something bad happened: %s", ex.what()); */
    /* } */

    const size_t serialOutputBufferLength = pack_payload_to_frame(buffer, bufferLength, serialOutputBuffer);
    uart_write_bytes(uart_num, (const char*)serialOutputBuffer, serialOutputBufferLength);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  free(buffer);
}
