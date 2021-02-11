#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
  while(1) {
    printf("Test for FreeRTOS loop\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
