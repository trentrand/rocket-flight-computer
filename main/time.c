#include "esp_sntp.h"
#include "esp_log.h"

extern const char *TAG;

void sntp_sync_time(struct timeval *tv) {
  settimeofday(tv, NULL);
  ESP_LOGI(TAG, "Time is synchronized from custom code");
  sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
}

void time_sync_notification_cb(struct timeval *tv) {
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void initialize_sntp(void) {
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  sntp_init();
}

void obtain_time(void) {
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

time_t synchronize_system_clock() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  if (timeinfo.tm_year <= 1970) {
    ESP_LOGI(TAG, "System clock is not set.");
    ESP_LOGI(TAG, "Connecting to Wifi and synchronizing time over SNTP.");
    obtain_time();
    time(&now);
  }

  char strftime_buf[64];

  // Debug log with local time (UTC -4 Eastern Time)
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current time in New York is: %s", strftime_buf);

  return now;
}

int64_t timestamp_microseconds() {
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}
