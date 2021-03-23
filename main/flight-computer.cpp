#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BNO055ESP32.h"
#include "sdkconfig.h"
#include "./models/telemetry.pb-c.h"
#include <string.h>
#include <stdio.h>

/* #include <string.h> */
/* #include "freertos/FreeRTOS.h" */
/* #include "freertos/task.h" */
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "BNO055ESP32.h"

static const char* TAG = "Flight Computer";

extern "C" {

// Event group defines bits with event statuses
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_num < CONFIG_ESP_WIFI_MAXIMUM_CONNECTION_RETRY) {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAG, "Attempting connection to Wifi AP");
    } else {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI(TAG,"Failed to connect to Wifi AP");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "Connected to Wifi! Device was assigned IP address:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void wifi_connect(void) {
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));

  wifi_config_t wifi_config = {
    .sta = {
      { .ssid = CONFIG_ESP_WIFI_SSID },
      { .password = CONFIG_ESP_WIFI_PASSWORD },
      .pmf_cfg = {
        .capable = true,
        .required = false
      }

    },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_start() );

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
   * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
      WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
      pdFALSE,
      pdFALSE,
      portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
   * happened. */
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Successfully connected to Wifi AP with SSID:%s",
        CONFIG_ESP_WIFI_SSID);
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "Failed to connect to SSID:%s",
        CONFIG_ESP_WIFI_SSID);
  } else {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }

  // The event will not be processed after unregister 
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
  vEventGroupDelete(s_wifi_event_group);
}

void app_main() {
  // Initialize non-volatile flash storage
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Perform network connection
  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_connect();

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
