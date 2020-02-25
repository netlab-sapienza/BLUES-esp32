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
#include "core_int.h"
  void app_main();
}

static bemesh_core_t *core1;

void server_routine(bemesh_core_t *core) {
  // while(1) {
  //   if(bemesh_core_is_scanning(core)) {
  //     bemesh_core_stop_scanning(core);
  //   }
  //   bemesh_core_start_advertising(core);
  //   vTaskDelay(500);
  // }
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
    printf("\nNo server found... launching server routine.\n");
    server_routine(core);
  } else {
    printf("\nFound a server, connecting...\n");
    bemesh_core_connect(core, bemesh_core_get_scan_result(core)->bda);
  }
  return;
}

// USED FOR MULTI_CONNECT PURPOSE ONLY
int next_conn_rdy=true;
// slightly advanced example that uses asynchronous calls.
static void on_scan_cmpl_cb(bemesh_evt_params_t *param) {
  // On scan complete event.
  // We want to execute multiple connections towards all the
  // found devices.
  for (int i = 0; i < param->scan.len; ++i) {
    if (next_conn_rdy) {
      printf("\nStarting connection proc.\n");
      bemesh_core_connect(core1, param->scan.result[i].bda);
      next_conn_rdy=false;    
    }    
  }
  return;
}

static void on_out_conn_cb(bemesh_evt_params_t *param) {
  // On connection happened event.
  char buf[64];
  int wb=sprintf(buf, "Connectd to ");
  for (int i = 0; i < 6; ++i) {
    wb+=sprintf(buf+wb, "%02X.", ((uint8_t *)param->conn.remote_bda)[i]);
  }
  sprintf(buf+wb, " with conn_id: %d\n", param->conn.conn_id);
  printf("%s", buf);

  // Test write event
  uint8_t sample_data[3] = {0xAB, 0xCD, 0xEF};
  printf("Testing write functionality.");
  bemesh_gattc_handler_write(core1->gattch, param->conn.conn_id,
			     sample_data, 3, true);
  next_conn_rdy=true;
  return;
}

void multi_connect_routine(bemesh_core_t *core) {
  bemesh_core_install_callback(core, ON_SCAN_END, on_scan_cmpl_cb);
  bemesh_core_install_callback(core, ON_OUT_CONN, on_out_conn_cb);
  while(core->outgoing_conn_len < 2) {
    if (!bemesh_core_is_scanning(core)) {
      printf("\nStart scanning proc.\n");
      bemesh_core_start_scanning(core, 3);
    }
    vTaskDelay(200);
  }  
  return;
}

//#define SRV_BHV 1

int main(void) {
  //bemesh::main_routine(NULL);
  core1=bemesh_core_init();
  vTaskDelay(100);

  multi_connect_routine(core1);
  //server_routine(core1);
  
  // #ifdef SRV_BHV
  // server_routine(core1);
  // #else
  // client_routine(core1);
  // #endif
  return 0;
}



void app_main(){
  main();
}
