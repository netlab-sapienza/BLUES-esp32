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


extern "C" {
  #include "kernel.h"
  void app_main();
 
}


bemesh::MessageHandler handler;
bemesh::Slave slave;
bemesh::Master master(10);


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
  slave.sayHello();
  
  ESP_LOGE(LOG_TAG, "Initializing project");
  ble_esp_startup();
  gatt_client_main();
  ESP_LOGE(LOG_TAG,"Success initializing client device");
  
  while(!has_ended_scanning());
  if(get_node_type() == SERVER){
    std::cout<<"I'm a server"<<std::endl;
    gatt_server_main();
    uint8_t gatt_if = get_gatt_if();
    uint8_t connection_id = get_client_connid();
    uint8_t* server_mac_address = get_my_MAC();
    master.set_device_connection_id(connection_id);
    master.set_dev_addr(server_mac_address);
    master.set_device_gatt_if(gatt_if);
    ESP_LOGE("SERVER","SUCCESS INITIALIZING SERVER DEVICE");
  }
  else if(get_node_type()  == CLIENT){

    //std::cout<<"I'm a client"<<std::endl;
    gatt_client_main();
    //TO-DO complete client device code.
    uint8_t gatt_if = get_gatt_if();
    uint8_t connection_id = get_client_connid();
    uint8_t* client_mac_address = get_my_MAC();
    slave.set_server_connection_id(connection_id);
    slave.set_dev_addr(client_mac_address);
    slave.set_device_gatt_if(gatt_if);
    ESP_LOGE("CLIENT","GATT_IF: %d CONNECTION_ID: %d MAC_ADDRESS: %p",gatt_if,connection_id,client_mac_address);
    ESP_LOGE("CLIENT","SUCCESS INITIALIZING CLIENT DEVICE");

  


  }
  else{
    std::cout<<"I'm NO ONE something's wrong"<<std::endl;  
    //TO-Do complete server device code.
  }
  



  return 0;
  
  
}



void app_main(){
  main();
}
