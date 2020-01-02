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
#include "routing.hpp"
#include "bemesh_messages.hpp"
#include "message_handler.hpp"



#include "slave.hpp"
#include "master.hpp"
#include "callbacks.hpp"

extern "C" {
  #include "kernel.h"
  void app_main();
 
}


bemesh::MessageHandler handler;

bool becoming_client;
bool becoming_server;


static void communication_message_callback(bemesh::MessageHeader* header,void * args){
  //TO DO: finish implementing all stuff
  int a  = 0;
  return;
}

static void routing_discovery_message_callback(bemesh::MessageHeader* header, void* args){
  //TO DO: finish implementing all stuff
  int a = 0;
  return;
}



static void* communication_message_callback_args;
static void* routing_discovery_message_callback_args;

#define LOG_TAG "be_mesh_demo"


int main(void) {
  becoming_client = false;
  becoming_server = false;
  


  //Installing messages on the application.
  bemesh::message_ops_t communication_message_ops{
    .message_id = COMMUNICATION_MESSAGE_ID,
    .recv_cb=communication_message_callback,
    .recv_cb_args = communication_message_callback_args,
  };

  bemesh::ErrStatus err = handler.installOps(&communication_message_ops);

  bemesh::message_ops_t routing_discovery_message_ops{
    .message_id = ROUTING_DISCOVERY_REQ_ID,
    .recv_cb = routing_discovery_message_callback,
    .recv_cb_args = routing_discovery_message_callback_args,
  };

  err = handler.installOps(&routing_discovery_message_ops);
  
  
  ESP_LOGE(LOG_TAG, "Initializing project");
  ble_esp_startup();
  gatt_client_main();
  bemesh::Callback callback_functor;
  //Installing all callbacks
  callback_functor();



  /*while(!(becoming_client || becoming_server));
  
  if(becoming_client){
    //reset global variable.
    becoming_client = false;
    ESP_LOGE(LOG_TAG, "I'm becoming a client");
    if(slave_istance == NULL){
      slave_istance = new bemesh::Slave();
      ESP_LOGE(LOG_TAG,"Building slave object on heap");
      uint8_t device_gatt_if = get_gatt_if();
      uint8_t* device_mac_address = get_my_MAC();
      uint8_t device_conn_id = get_client_connid();
      slave_istance->set_device_gatt_if(device_gatt_if);
      slave_istance->set_dev_addr(device_mac_address);
      slave_istance->set_server_connection_id(device_conn_id);
      //slave_istance->print_status();
    }
  }
  else if(becoming_server){
    becoming_server = false;
    ESP_LOGE(LOG_TAG, "I'm becoming a server");
    if(master_istance == NULL){
      master_istance = new bemesh::Master();
      ESP_LOGE(LOG_TAG,"Building master object on heap");
      uint8_t device_gatt_if = get_gatt_if();
      uint8_t* device_mac_address = get_my_MAC();
      uint8_t device_conn_id = get_client_connid();
      master_istance->set_device_gatt_if(device_gatt_if);
      master_istance->set_dev_addr(device_mac_address);
      master_istance->set_device_connection_id(device_conn_id);
      ESP_LOGE(LOG_TAG,"DEVICE CONN_ID: %d",master_istance->get_device_connection_id());

    }
  }
*/
 
  return 0;
  
  
}



void app_main(){
  main();
}
