/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */
#include <stdio.h>
#include <vector>
#include <iostream>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"


//
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"
#include "server.hpp"

#define GATTS_TAG "server_demo"


static const char* LOG_TAG = "main";


int main(void) {
  std::cout<<"Pippo"<<std::endl;
  esp_err_t ret;
  // Initialize NVS.
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
      ESP_LOGE(GATTS_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
      return 1;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
      ESP_LOGE(GATTS_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
      return 1;
  }
  ret = esp_bluedroid_init();
  if (ret) {
      ESP_LOGE(GATTS_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
      return 1;
  }
  ret = esp_bluedroid_enable();
  if (ret) {
      ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
      return 1;
  }

  //Initialize Server object
  Server server(1,"esp1");
  std::cout<<"Initializing server object"<<std::endl;
 
  return 0;
}






extern "C" {
  void app_main();
}
void app_main(){
  main();
}
