#include "server.hpp"
#include <iostream>

#define GATTS_TAG "server_demo"


void gap_event_handler_trampoline(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){}

/*
void Server::sayHello(){
    std::cout<<"Hello server"<<std::endl;
}*/

Server::Server(uint8_t id, std::string ta):serverId(id),TAG(ta){ 
}

Server::Server(uint8_t id):serverId(id){}


std::string Server::getTAG(){return TAG;}

uint8_t Server::getId(){return serverId;}

esp_gatt_rsp_t Server::read_characteristic(struct gatts_profile_inst* table, 
                            unsigned int table_size,
                            uint16_t service_handle, uint16_t char_handle,
                            esp_ble_gatts_cb_param_t *param){
    esp_gatt_rsp_t rsp;
    if(table == NULL)
        exit(1);
    int i;
    for(i = 0;i<table_size; i++){
        if(table[i].service_handle == service_handle){
            
            rsp.attr_value.handle = param->read.handle;

            //Constant values for now.
            rsp.attr_value.len = 2;
            rsp.attr_value.value[0] = 0x12;
            rsp.attr_value.value[1] = 0x34;
            return rsp;
        }
    }
    return rsp;
}

/*
int write_characteristic(struct gatts_profile_inst* table,
                            unsigned int table_size, uint16_t service_handle, 
                            uint16_t char_handle, void* value);
*/          


