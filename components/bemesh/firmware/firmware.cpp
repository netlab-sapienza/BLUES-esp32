/*
 * firmware.cpp
 * Main routines to be executed on the esp32 board
 */

#include "firmware.hpp"

namespace bemesh {
  void main_routine(void* args) {
    // May use additional args if needed.

    //bool becoming_client=false;
    //bool becoming_server=false;
    const char* LOG_TAG = "be_mesh_demo";

    ESP_LOGE(LOG_TAG, "Initializing Firmware...");
    
	
    //Installing all callbacks
    bemesh::Callback callback_functor;
    callback_functor();



    ble_esp_startup();
    gatt_client_main();
    //gatt_server_main();
    
      
    return;
  }
}
