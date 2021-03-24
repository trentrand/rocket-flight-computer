#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sntp.h"
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

  // Waiting until either the connection is established (WIFI_CONNECTED_BIT)
  // or connection failed for the maximum number of re-tries (WIFI_FAIL_BIT)
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
      WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
      pdFALSE,
      pdFALSE,
      portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Successfully connected to Wifi AP with SSID: %s",
        CONFIG_ESP_WIFI_SSID);
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "Failed to connect to SSID: %s",
        CONFIG_ESP_WIFI_SSID);
  } else {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }

  // The event will not be processed after unregister
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
  vEventGroupDelete(s_wifi_event_group);
}

void sntp_sync_time(struct timeval *tv) {
  settimeofday(tv, NULL);
  ESP_LOGI(TAG, "Time is synchronized from custom code");
  sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
}

void time_sync_notification_cb(struct timeval *tv) {
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}

static void initialize_sntp(void) {
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  sntp_init();
}

static void obtain_time(void) {
  initialize_sntp();
  time_t now = 0;
  struct tm timeinfo = { 0 };

  int retry = 0;
  const int retry_count = 10;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  time(&now);
  localtime_r(&now, &timeinfo);
}

extern "C" void app_main() {
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

  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  if (timeinfo.tm_year <= 1970) {
    ESP_LOGI(TAG, "System clock is not set. Connecting to Wifi and getting time over NTP.");
    obtain_time();
    time(&now);
  }

  char strftime_buf[64];

  // Set timezone to Eastern Standard Time and print local time
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current time in New York is: %s", strftime_buf);

  // Store timestamp
  telemetry.timestampstart = now;

  // Serialize
  unsigned char simple_pad[8];
  unsigned char *buffer;
  size_t bufferLength;
  ProtobufCBufferSimple bs = PROTOBUF_C_BUFFER_SIMPLE_INIT(simple_pad);

  bufferLength = telemetry__get_packed_size(&telemetry);
  buffer = (unsigned char*) malloc(bufferLength);

  telemetry__pack(&telemetry, buffer);
  telemetry__pack_to_buffer(&telemetry, &bs.base);

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

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

    fprintf(stderr, "Writing %hu serialized bytes\n", bufferLength);
    printf("Timestamp: %llu\n", time_us);
    fwrite(buffer, bufferLength, 1, stdout);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  PROTOBUF_C_BUFFER_SIMPLE_CLEAR(&bs);
  free(buffer);
}
