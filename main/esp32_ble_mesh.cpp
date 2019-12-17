/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */
#include <stdio.h>
#include <vector>

#include "__message_parser.h"
#include "routing.h"

#include "esp_log.h" // for logging
#include "nvs_flash.h" // Non volatile interface
#include "esp_bt.h" // Implements BT controller and VHCI configuration procedures
#include "esp_bt_main.h" // Implements initialization and enabling of Bluedroid stack
#include "esp_gap_ble_api.h" // implements GAP configuration
#include "esp_gatts_api.h" // Implements GATT configuration




static const char* LOG_TAG = "main";

int main(void) {
  esp_err_t ret;  
  /*
  bemesh::Router r1(0x06);
  bemesh::Router r2(0x1C);
  // Insert two clients with their static addresses
  // in the routing table of r1
  // We expect the following output
  // Network address / static address
  // server addr : 0x06 (becomes 0x30 as network id)
  // 0x30 -> 0xAA
  // 0x31 -> 0xCF  
  r1.add(0xAA, bemesh::Client);
  r1.add(0xCF, bemesh::Client);
  
  // server addr : 0x1C (becomes 0xE0 as network id)
  // 0xE0 -> 0x04
  // 0xE1 -> 0xAB  
  r2.add(0x04, bemesh::Client);
  r2.add(0xAB, bemesh::Client);

  ESP_LOGI(LOG_TAG, "Router 1 summary:");
  ESP_LOGI(LOG_TAG, "own_addr: %X\tnum_clients: %d\tnum_servers: %d",
	   r1.m_server_addr, r1.m_client_num, r1.m_server_num);
  for(auto& row:r1.rtable.getTable()) {
    ESP_LOGI(LOG_TAG, "%X -> %X", row.first, row.second);
  }

  ESP_LOGI(LOG_TAG, "Router 2 summary:");
  ESP_LOGI(LOG_TAG, "own_addr: %d\tnum_clients: %d\tnum_servers: %d",
	   r2.m_server_addr, r2.m_client_num, r2.m_server_num);
  for(auto& row:r2.rtable.getTable()) {
    ESP_LOGI(LOG_TAG, "%X  -> %X", row.first, row.second);
  }
  */
  

  
  /*
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

  */
  
  return 0;
}






extern "C" {
  void app_main();
}
void app_main(){
  main();
}
