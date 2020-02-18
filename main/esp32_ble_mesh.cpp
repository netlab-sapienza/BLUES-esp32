/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */

//#include "firmware.hpp"



#define PROFILE_A_APP_ID 0
#define PROFILE_B_APP_ID 1

extern "C" {
  // These two libraries are needed for vTaskDelay
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gap_handler.h"
#include "gatts_handler.h"
#include "gattc_handler.h"
#include "core.h"
  void app_main();
}

int main(void) {
  //bemesh::main_routine(NULL);
  bemesh_core_t *core1=bemesh_core_init();
  bemesh_gap_handler_mode(core1->gaph, GAP_HANDLER_MODE_CENTRAL);
  esp_bd_addr_t remote_bda1 = {0x24, 0x6f, 0x28, 0x97, 0x4c, 0x32};
  vTaskDelay(500);
  bemesh_gattc_open(core1->gattch, remote_bda1, BLE_ADDR_TYPE_PUBLIC);

  /*
    vTaskDelay(4000);
  esp_bd_addr_t server_bda={0x24,0x6f,0x28,0x96,0x8c,0xaa};
  bemesh_gattc_open(gattc_handle, server_bda, BLE_ADDR_TYPE_PUBLIC);
  */
  return 0;
}



void app_main(){
  main();
}
