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

    MessageHandler* Master::get_message_handler(){
        return &mes_handler;
    }

    Router* Master::get_router(){
        return router;
    }

    uint8_t* Master::get_master_tx_buffer(){
        return master_tx_buffer;
    }

    //Master object main task. Used to test functions and primitives.
    void Master::start(){

        //Can't start if master is not allocated.
        if(master_istance == NULL)
            return;
        uint16_t  gatt_if = get_gatt_if();
        uint8_t* mac_address = get_my_MAC();
        uint8_t conn_id = get_client_connid();

        
        set_device_gatt_if(gatt_if);
        set_device_connection_id(conn_id);
        set_dev_addr(mac_address);
        dev_addr_t addr;
        if(mac_address != NULL){
            addr = _build_dev_addr(mac_address);
            //Router object allocation.
            router = new Router(addr);
        }

        //Buffer to send/receive messages.
        mes_handler.installTxBuffer(master_tx_buffer);
        

        //Implement the transmission and receive callbacks.

        const char* fake_message = "HELLO";
        uint16_t BUF_SIZE = 256;
        uint8_t buff[BUF_SIZE];


        //Error. It returns -1.
        write_characteristic(IDX_CHAR_A,addr,(void*)fake_message,5,device_gatt_if,device_conn_id);




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
                                        uint8_t buffer_size, uint16_t gatts_if,uint16_t conn_id)
    {
        if(buffer == NULL)
            return WrongPayload;
        if(conn_id != device_conn_id)
            return WrongAddress;

        if(characteristic == IDX_CHAR_A || characteristic == IDX_CHAR_B ||
            characteristic == IDX_CHAR_C )
        {
            write_CHR(gatts_if,conn_id,characteristic,(uint8_t*)buffer,buffer_size);
            return Success;
        }        
        
        else
        {
            return GenericError;
        }


    }

    void Master::shutdown(){
        if(master_istance != NULL)
            delete master_istance;
    }
    
    /*
        Private functions
    */
    dev_addr_t Master::_build_dev_addr(uint8_t* address){
        int i;
        dev_addr_t ret;
        for(i = 0; i<DEV_ADDR_LEN; ++i){
            ret[i] = address[i];
        }
        return ret;
    }

    
}
