/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */

#define PROFILE_A_APP_ID 0
#define PROFILE_B_APP_ID 1





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


//Mid-tier. Routing and message parsing.


#include "slave.hpp"
#include "master.hpp"
#include "callbacks.hpp"
#include "bemesh_error.hpp"



extern "C" {
  #include "kernel.h"
  void app_main();
 
}


bool becoming_client;
bool becoming_server;
int bemesh_errno;

#define LOG_TAG "be_mesh_demo"


int main(void) {
  becoming_client = false;
  becoming_server = false;

  //Set the global error variable to a default value.
  bemesh_errno = SUCCESS;

  ESP_LOGE(LOG_TAG, "Initializing project");
  ble_esp_startup();
  gatt_client_main();
  //The two ESP become server.
  //gatt_server_main();
  bemesh::Callback callback_functor;
  //Installing all callbacks
  callback_functor();
  return 0;
  
  
}



void app_main(){
  main();
}
