/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */
#include <stdio.h>
#include <vector>

#include "__message_parser.h"

#include "esp_log.h" // for logging
#include "nvs_flash.h" // Non volatile interface
#include "esp_bt.h" // Implements BT controller and VHCI configuration procedures
#include "esp_bt_main.h" // Implements initialization and enabling of Bluedroid stack
#include "esp_gap_ble_api.h" // implements GAP configuration
#include "esp_gatts_api.h" // Implements GATT configuration




static const char* LOG_TAG = "main";

static void gatts_event_handler();

static void gap_event_handler();

int main(void) {
  esp_err_t ret;
  // Initalize non volatile storage lib
  ret = nvs_flash_init();

  // BT controller configuration structure
  // Implements HCI controller interface, LL and PHY layers
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(LOG_TAG, "%s, initialize controller failed\n", __func__);
  }

  // Set BLE Mode
  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);

  // Enable Bluedroid
  ret = esp_bluedroid_init();
  ret = esp_bluedroid_enable();

  // Pass the handlers callback functions to bluedroid
  //esp_ble_gatts_register_callback(gatts_event_handler);
  //esp_ble_gap_register_callback(gap_event_handler);

  
  return 0;
}






extern "C" {
  void app_main();
}
void app_main(){
  main();
}
