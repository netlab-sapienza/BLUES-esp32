#include "master.hpp"
#include <iostream>



namespace bemesh{
  

    Master::Master(uint8_t id, std::string name):name(name),device_conn_id(id){ 
    }

    Master::Master(uint8_t id):device_conn_id(id){
    }

    Master::Master(){
        
    }

    

    Master::~Master(){

    }


    std::string Master::get_name(){
        return name;
    }

    void Master::set_name(std::string new_name){
        name = new_name;
        return;
    }


     bool Master::is_connected_to_internet(){
        return connected_to_internet;
    }

    void Master::set_connected_to_internet(bool connected){
        connected_to_internet = connected;
        return;
    }


    bool Master::is_esp(){
        return esp;
    }

    void Master::set_esp(bool is_esp){
        esp = is_esp;
        return;
    }

    uint8_t* Master::get_dev_addr(){
        return address;
    }

    void Master::set_dev_addr(uint8_t* dev_addr){
        if(esp)
            address = dev_addr;
        return;
    }


    uint16_t Master::get_device_connection_id(){
        return device_conn_id;
    }

    void Master::set_device_connection_id(uint16_t conn_id){
        device_conn_id = conn_id;
    }

    uint8_t Master::get_device_gatt_if(){
        return device_gatt_if;
    }

    void Master::set_device_gatt_if(uint16_t gatt_if){
        device_gatt_if = gatt_if;
    }


    int16_t Master::read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id)
    {
        if(buffer == NULL)
            return -1;
        if(characteristic == IDX_CHAR_A || characteristic == IDX_CHAR_B || 
            characteristic == IDX_CHAR_C)
        {

            uint8_t* received_bytes=  read_CHR(gattc_if,conn_id,characteristic);
            uint8_t char_len_read = get_CHR_value_len(characteristic);
            if(buffer_size < char_len_read){
                return -1;
            }
            memcpy((void*)received_bytes,buffer,char_len_read);
            return char_len_read;

        }
        else
            return -1;    

    }




    ErrStatus Master::write_characteristic(uint8_t characteristic, dev_addr_t address, void* buffer,
                                        uint8_t buffer_size, uint16_t gattc_if,uint16_t conn_id)
    {
        if(buffer == NULL)
            return WrongPayload;
        if(conn_id != device_conn_id)
            return WrongAddress;

        if(characteristic == IDX_CHAR_A || characteristic == IDX_CHAR_B ||
            characteristic == IDX_CHAR_C )
        {
            write_CHR(gattc_if,conn_id,characteristic,(uint8_t*)buffer,buffer_size);
            return Success;
        }        
        
        else
        {
            return GenericError;
        }

    }










    
}
