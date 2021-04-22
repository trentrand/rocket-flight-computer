#include <stdio.h>
#include "bno055.h"

#define BNO_POLLING_MS 50

i2c_number_t i2c_num = 0;

void initialize_bno055() {
  bno055_config_t bno_conf;

  esp_err_t err;
  err = bno055_set_default_conf(&bno_conf);
  err = bno055_open(i2c_num, &bno_conf);
  printf("bno055_open() returned 0x%02X \n", err);

  if( err != ESP_OK ) {
    printf("Program terminated!\n");
    err = bno055_close(i2c_num);
    printf("bno055_close() returned 0x%02X \n", err);
    exit(1);
  }

  vTaskDelay(1000 / portTICK_RATE_MS);

  err = bno055_set_opmode(i2c_num, OPERATION_MODE_NDOF);
  printf("bno055_set_opmode(OPERATION_MODE_NDOF) returned 0x%02x \n", err);
  vTaskDelay(1000 / portTICK_RATE_MS);

  //TODO wait for calibration
}
