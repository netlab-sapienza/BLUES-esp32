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
#include "core.h"
  void app_main();
}

int main(void) {
  //bemesh::main_routine(NULL);
  bemesh_core_init();
  bemesh_gap_handler* gap_handle=bemesh_gap_handler_init(NULL, 0,
    NULL, 0);
  bemesh_gatts_handler* gatts_handle=bemesh_gatts_handler_init();
  bemesh_gap_handler_mode(gap_handle, GAP_HANDLER_MODE_PERIPHERAL);
  vTaskDelay(1000);
  bemesh_gap_handler_mode(gap_handle, GAP_HANDLER_MODE_CENTRAL);
  return 0;
}



void app_main(){
  main();
}
