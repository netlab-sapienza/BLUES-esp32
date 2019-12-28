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
#include "master.hpp"
#include "routing.hpp"

#include "bemesh_messages.hpp"
#include "message_handler.hpp"
#include "slave.hpp"

#include "kernel.h"

extern "C" {
  
  void app_main();
 
}




#define GATTS_TAG "GATTS_DEMO"



bemesh::MessageHandler handler;


static void communication_message_callback(bemesh::MessageHeader* header,void * args){
    int a  = 0;
    return;
}

static void* communication_message_callback_args;

#define LOG_TAG "be_mesh_demo"


int main(void) {
    ESP_LOGE(LOG_TAG, "Initializing project");
    ble_esp_startup();
    gatt_client_main();
    ESP_LOGE(LOG_TAG,"Success initializing client device");

    bemesh::message_ops_t communication_message_ops{
        .message_id = COMMUNICATION_MESSAGE_ID,
        .recv_cb=communication_message_callback,
        .recv_cb_args = communication_message_callback_args,
    };
    bemesh::ErrStatus err = handler.installOps(&communication_message_ops);

    




    return 0;
  
  
}



void app_main(){
  main();
}
