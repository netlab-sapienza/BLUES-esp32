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

void server_routine(bemesh_core_t *core) {
  // while(1) {
  //   if(bemesh_core_is_scanning(core)) {
  //     bemesh_core_stop_scanning(core);
  //   }
  //   bemesh_core_start_advertising(core);
  //   vTaskDelay(500);
  if(bemesh_core_is_scanning(core)) {
    bemesh_core_stop_scanning(core);
  }
  bemesh_core_start_advertising(core);  
  return;
}
void client_routine(bemesh_core_t *core) {
  uint8_t scan_timeout=esp_random()%8;
  if(!scan_timeout) {
    scan_timeout+=1;
  }
  printf("\nStarting scan proc. with timeout %d\n", scan_timeout);
  bemesh_core_start_scanning(core, scan_timeout);
  while(!bemesh_core_scan_complete(core)) {}
  uint8_t scan_res_len = bemesh_core_get_scan_result_len(core);
  if(!scan_res_len) {
    server_routine(core);
  }
  printf("\nFound a server, connecting...\n");
  bemesh_core_connect(core, bemesh_core_get_scan_result(core)->bda);
  return;
}

//#define SRV_BHV 1

int main(void) {
  //bemesh::main_routine(NULL);
  bemesh_core_t *core1=bemesh_core_init();
  vTaskDelay(100);
  #ifdef SRV_BHV
  server_routine(core1);
  #else
  client_routine(core1);
  #endif
  return 0;
}



void app_main(){
  main();
}
