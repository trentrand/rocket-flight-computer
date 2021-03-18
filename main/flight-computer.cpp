#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BNO055ESP32.h"
#include "sdkconfig.h"
#include <string.h>

#include <cstdint>
#include "capnp_c.h"
#include "./models/telemetry.capnp.h"

static const char* TAG = "BNO055";

extern "C" void app_main() {
  BNO055 bno(UART_NUM_1, (gpio_num_t)CONFIG_BNO055_SCL_GPIO, (gpio_num_t)CONFIG_BNO055_SDA_GPIO);

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

  uint8_t buf[4096];

  while (1) {
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

    struct capn c;
    capn_init_malloc(&c);
    capn_ptr cr = capn_root(&c);
    struct capn_segment *cs = cr.seg;

    struct Telemetry logs = {
      .timestampStart = time_us,
    };
    // Set initial object in `p`.
    Telemetry_ptr telemetryPointer = new_Telemetry(cs);
    write_Telemetry(&logs, telemetryPointer);
    capn_setp(capn_root(&c), 0, telemetryPointer.p);
    capn_write_mem(&c, buf, sizeof(buf), 0 /* packed */);
    capn_free(&c);

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
