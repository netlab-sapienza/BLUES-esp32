#include "slave.hpp"
#include "kernel.h"
//TO-DO. complete stuff.


namespace bemesh{
    Slave::Slave(){
        esp = true;
        connected_to_internet = false;
        device_id = 0;
        
        
        //dev_addr_t = 0;

    }

    Slave::Slave(bool is_esp, bool connected_to_internet):esp(is_esp), connected_to_internet(connected_to_internet){
        device_id = 0;

        /*
        if(is_esp)
            address = 0;
        else
        {
            //disable mac address recognition
            address = 0;
        }
         */           

    }

    Slave::~Slave(){

    }


    std::string Slave::get_name(){
        return name;
    }

    void Slave::set_name(std::string new_name){
        name = new_name;
        return;
    }


    bool Slave::is_connected_to_internet(){
        return connected_to_internet;
    }

    void Slave::set_connected_to_internet(bool connected){
        connected_to_internet = connected;
        return;
    }


    bool Slave::is_esp(){
        return esp;
    }

    void Slave::set_esp(bool is_esp){
        esp = is_esp;
        return;
    }

    dev_addr_t Slave::get_dev_addr(){
        return address;
    }

    void Slave::set_dev_addr(dev_addr_t dev_addr){
        if(esp)
            address = dev_addr;
    }


    uint16_t Slave::get_server_connection_id(){
        return server_conn_id;
    }

    void Slave::set_server_connection_id(uint16_t conn_id){
        server_conn_id = conn_id;
    }


    MessageHandler Slave::get_message_handler(){
        return msg_handler;
    }

    





    int16_t Slave::read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
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


    ErrStatus Slave::send_message(uint16_t gattc_if,uint16_t conn_id,char* message,
                                        uint8_t message_size)
    {
        if(message == NULL || message_size == 0){
            return WrongPayload;
        }
        if(conn_id != this->server_conn_id)
            return WrongAddress;
        else
        {
            write_CHR(gattc_if,conn_id,IDX_CHAR_A,(uint8_t *)message,message_size);
            return Success;
        }
        //Implementare una sorta di meccanismo di acknowledgement se possibile.
        
            

    }



    ErrStatus Slave::write_characteristic(uint8_t characteristic, dev_addr_t address, void* buffer,
                                        uint8_t buffer_size, uint16_t gattc_if,uint16_t conn_id)
    {
        if(buffer == NULL)
            return WrongPayload;
        if(conn_id != server_conn_id)
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
