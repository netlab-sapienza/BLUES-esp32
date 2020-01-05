

#include "common.hpp"


task_param_write_t::task_param_write_t(){}



bool contains_mac(std::list<uint8_t*> ls,uint8_t* address, uint8_t address_size){
    std::list<uint8_t*>::iterator it;
    for(it = ls.begin(); it != ls.end(); ++it){
        uint8_t * addr = *it;
        int i;
        bool found = true;
        for(i = 0;i<address_size; i++){
            if(addr[i] != address[i])
                found = false;
        }
        if(found)
            return true;
    }
    return false;

}


bemesh::dev_addr_t _build_dev_addr(uint8_t* address){
        int i;
        bemesh::dev_addr_t ret;
        for(i = 0; i<DEV_ADDR_LEN; ++i){
            ret[i] = address[i];
        }
        return ret;
    }
void _print_mac_address(uint8_t* address){
    int i;
    for(i = 0; i<MAC_ADDRESS_SIZE;i++){
        ESP_LOGE("CLIENT","Byte[%d]: %x",i,address[i]);
    }
}



//A task used to perform writing on charateristics. In esp-idf task ~ thread.
void write_characteristic_task(void * task_param) {
    task_param_write_t* params = (task_param_write_t*)task_param;
   
    uint8_t conn_id = params->conn_id;
    uint16_t gatt_if = params->gatt_if;
    uint8_t charact = params->characteristic;
    uint8_t * data = params->buffer;
    uint16_t buffer_size = params->buffer_size;
    
    write_CHR(gatt_if, conn_id, charact, data, buffer_size);
    std::cout<<"I wrote"<<std::endl;
    uint8_t * test = read_CHR(gatt_if,conn_id, charact);
    int i;
    for(i = 0; i<buffer_size; i++){
        ESP_LOGE(GATTS_TAG,"Found: %d",test[i]);
    }
  
    vTaskDelete(NULL);

}
    