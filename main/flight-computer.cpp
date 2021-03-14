#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BNO055ESP32.h"
#include "sdkconfig.h"
#include "./models/telemetry.pb-c.h"
#include <string.h>
#include <stdio.h>

static const char* TAG = "BNO055";

extern "C" void app_main() {
  BNO055 bno(UART_NUM_1, (gpio_num_t)CONFIG_BNO055_SCL_GPIO, (gpio_num_t)CONFIG_BNO055_SDA_GPIO);

  // Initialize Telemetry proto buffer
  Telemetry telemetry = TELEMETRY__INIT;

  // Store timestamp
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

  telemetry.timestampstart = time_us;

  // Serialize
  unsigned char simple_pad[8];
  unsigned char *buffer;
  size_t bufferLength;
  ProtobufCBufferSimple bs = PROTOBUF_C_BUFFER_SIMPLE_INIT(simple_pad);

  bufferLength = telemetry__get_packed_size(&telemetry);
  buffer = (unsigned char*) malloc(bufferLength);

  telemetry__pack(&telemetry, buffer);
  telemetry__pack_to_buffer(&telemetry, &bs.base);

  fprintf(stderr, "Writing %hu serialized bytes\n", bufferLength);

  PROTOBUF_C_BUFFER_SIMPLE_CLEAR(&bs);
  free(buffer);

  try {
    bno.begin();
    bno.enableExternalCrystal();
    bno.setOprModeNdof();
    ESP_LOGI(TAG, "Setup Done.");
  } catch (BNO055BaseException& ex) {
    ESP_LOGE(TAG, "Setup Failed, Error: %s", ex.what());
    return;
  } catch (std::exception& ex) {
    ESP_LOGE(TAG, "Setup Failed, Error: %s", ex.what());
    return;
  }

  try {
    int8_t temperature = bno.getTemp();
    ESP_LOGI(TAG, "TEMP: %d°C", temperature);

    int16_t sw = bno.getSWRevision();
    uint8_t bl_rev = bno.getBootloaderRevision();
    ESP_LOGI(TAG, "SW rev: %d, bootloader rev: %u", sw, bl_rev);

    bno055_self_test_result_t res = bno.getSelfTestResult();
    ESP_LOGI(TAG, "Self-Test Results: MCU: %u, GYR:%u, MAG:%u, ACC: %u", res.mcuState, res.gyrState, res.magState,
        res.accState);
  } catch (BNO055BaseException& ex) {
    ESP_LOGE(TAG, "Something bad happened: %s", ex.what());
    return;
  } catch (std::exception& ex) {
    ESP_LOGE(TAG, "Something bad happened: %s", ex.what());
    return;
  }

  while (1) {
    try {
      bno055_calibration_t cal = bno.getCalibration();
      bno055_vector_t v = bno.getVectorEuler();
      ESP_LOGI(TAG, "Euler: X: %.1f Y: %.1f Z: %.1f || Calibration SYS: %u GYRO: %u ACC:%u MAG:%u", v.x, v.y, v.z, cal.sys,
          cal.gyro, cal.accel, cal.mag);
    } catch (BNO055BaseException& ex) {
      ESP_LOGE(TAG, "Something bad happened: %s", ex.what());
      return;
    } catch (std::exception& ex) {
      ESP_LOGE(TAG, "Something bad happened: %s", ex.what());
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
