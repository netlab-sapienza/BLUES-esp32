#include "server.hpp"
#include <iostream>



namespace bemesh{
    void gap_event_handler_trampoline(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){}

    /*
    void Server::sayHello(){
        std::cout<<"Hello server"<<std::endl;
    }*/

    Server::Server(uint8_t id, std::string ta):serverId(id),TAG(ta){ 
    }

    Server::Server(uint8_t id):serverId(id){
        esp_gatt_perm_t next_service_id_permissions = 0;
        next_service_id_permissions |= ESP_GATT_PERM_READ;
        next_service_id_permissions |= ESP_GATT_PERM_WRITE;
        std::string nextIdName = "Next id service";
        bool is_next_id_characteristic = true;

        nextIdService.perms = next_service_id_permissions;
        nextIdService.name = nextIdName;
        nextIdService.is_characteristic = is_next_id_characteristic;
        

    }


    std::string Server::getTAG(){return TAG;}

    uint8_t Server::getId(){return serverId;}




    esp_gatt_rsp_t Server::read_characteristic(gatts_profile_inst* table, 
                                unsigned int table_size,
                                uint16_t service_handle, uint16_t char_handle,
                                esp_ble_gatts_cb_param_t *param){
            
        esp_gatt_rsp_t rsp;
        std::cout<<"Next id service: "<<nextIdService.nextId<<std::endl;
        nextIdService.nextId++;
        std::cout<<"Next id service: "<<nextIdService.nextId<<std::endl;
        if(table == NULL)
            exit(1);
        int i;
        /*
        for(i = 0;i<table_size; i++){
            if(table[i].service_handle == service_handle){
                
                rsp.attr_value.handle = param->read.handle;

                //Constant values for now.
                rsp.attr_value.len = 2;
                rsp.attr_value.value[0] = 0x12;
                rsp.attr_value.value[1] = 0x34;
                return rsp;
            }
        }*/
        return rsp;
    }




    /*
    int write_characteristic(struct gatts_profile_inst* table,
                                unsigned int table_size, uint16_t service_handle, 
                                uint16_t char_handle, void* value);
    */          

}
