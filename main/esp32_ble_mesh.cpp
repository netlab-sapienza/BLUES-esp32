/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */
#include <stdio.h>
#include <vector>

#include "esp_log.h" // for logging

#include "bemesh.h"


static const char* LOG_TAG = "main";

int main(void) {
  int err;
  ESP_LOGI(LOG_TAG, "Hello World!");
  applyRoutine();
  return 0;
}






extern "C" {
  void app_main();
}
void app_main(){
  main();
}
