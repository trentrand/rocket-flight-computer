#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <driver/gpio.h>
#include <driver/uart.h>

/* #include "BNO055ESP32.h" */
/* #include "./models/telemetry.pb-c.h" */

#include "./time.c"
#include "./flash-storage.c"
#include "./network.c"

const char* TAG = "Flight Computer";

void app_main() {
  initialize_flash_storage();
  wifi_connect();

  /* BNO055 bno(UART_NUM_1, (gpio_num_t)CONFIG_BNO055_SCL_GPIO, (gpio_num_t)CONFIG_BNO055_SDA_GPIO); */

  // Initialize Telemetry proto buffer
  /* Telemetry telemetry = TELEMETRY__INIT; */

  // Store timestamp
  time_t now = time_now();
  /* telemetry.timestampstart = now; */

  // Serialize
  uint8_t *buffer;
  uint8_t *serialOutputBuffer;

  const int bufferLength = 16;
  /* bufferLength = telemetry__get_packed_size(&telemetry); */
  buffer = (uint8_t*) heap_caps_calloc(1, bufferLength, MALLOC_CAP_8BIT);
  serialOutputBuffer = (uint8_t*) heap_caps_calloc(1, bufferLength, MALLOC_CAP_8BIT);
  for (size_t index = 0; index < bufferLength; index++) {
    buffer[index] = 0;
  }

  const int uart_num = UART_NUM_0;
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };

  uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  ESP_ERROR_CHECK(uart_driver_install(uart_num, 128 * 2, 0, 0, NULL, 0));

  /* telemetry__pack(&telemetry, buffer); */

  // Configure UART parameters
  /* ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config)); */
  /* uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); */

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

  while (1) {
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

    int64_t time_us = timestamp_microseconds();
    ESP_LOGI(TAG, "Timestamp: %llu\n", time_us);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
