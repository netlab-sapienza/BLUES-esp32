#include "slave.hpp"

//TO-DO. complete stuff.


namespace bemesh{
    Slave::Slave(){
        esp = true;
        connected_to_internet = false;

    }

    Slave::Slave(bool is_esp, bool connected_to_internet):esp(is_esp), connected_to_internet(connected_to_internet){
        
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

    uint8_t* Slave::get_dev_addr(){
        return address;
    }

    void Slave::set_dev_addr(uint8_t* dev_addr){
        if(esp)
            address = dev_addr;
    }


    uint16_t Slave::get_server_connection_id(){
        return server_conn_id;
    }

    void Slave::set_server_connection_id(uint16_t conn_id){
        server_conn_id = conn_id;
    }

    uint16_t Slave::get_device_connection_id(){
        return device_conn_id;
    }

    void Slave::set_device_connection_id(uint16_t conn_id){
        device_conn_id = conn_id;
    }

    uint8_t Slave::get_device_gatt_if(){
        return device_gatt_if;
    }

    void Slave::set_device_gatt_if(uint16_t gatt_if){
        device_gatt_if = gatt_if;
    }

    Router* Slave::get_router(){
        return router;
    }

    MessageHandler* Slave::get_message_handler(){
        return &mes_handler;
    }

    uint8_t* Slave::get_slave_tx_buffer(){
        return slave_tx_buffer;
    }


    dev_addr_t Slave::_build_dev_addr(uint8_t* address){
        int i;
        dev_addr_t ret;
        for(i = 0; i<DEV_ADDR_LEN; ++i){
            ret[i] = address[i];
        }
        return ret;
    }





    int16_t Slave::read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id)
    {
        if(buffer == NULL)
            return -1;
        if(characteristic == IDX_CHAR_VAL_A || characteristic == IDX_CHAR_VAL_B || 
            characteristic == IDX_CHAR_VAL_C)
        {

            uint8_t* received_bytes=  read_CHR(gattc_if,conn_id,characteristic);
            uint8_t char_len_read = get_CHR_value_len(characteristic);
            /*if(buffer_size < char_len_read){
                return -1;
            }*/
            memcpy((void*)received_bytes,buffer,char_len_read);
            std::cout<<"Byte[0]: "<<received_bytes[0]<<std::endl;
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
            //write_CHR(gattc_if,conn_id,IDX_CHAR_A,(uint8_t *)message,message_size);
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

        if(characteristic == IDX_CHAR_VAL_A || characteristic == IDX_CHAR_VAL_B ||
            characteristic == IDX_CHAR_VAL_C )
        {
            write_CHR(gattc_if,conn_id,characteristic,(uint8_t*)buffer,buffer_size);
            return Success;
        }        
        
        else
        {
            return GenericError;
        }

    }
    
    void Slave::shutdown(){
        if(slave_istance != NULL)
            delete slave_istance;
    }

    void Slave::start(){
        if(slave_istance == NULL)
            return;
        
        //Initialize all fields of the class;
        uint16_t gatt_if = get_gatt_if();
        uint8_t* mac_address = get_my_MAC();
        uint8_t conn_id = get_client_connid();
        _print_mac_address(mac_address);

        set_device_gatt_if(gatt_if);
        set_device_connection_id(conn_id);
        set_dev_addr(mac_address);

        dev_addr_t converted_address = _build_dev_addr(mac_address);  
        if(mac_address != NULL){
            router = new Router(converted_address);
        }

        mes_handler.installTxBuffer(slave_tx_buffer);

        const char* fake_message = "HELLO";
        std::cout<<"I'm about to write smth"<<std::endl;
        int i;

        //Characteristic subscription?
        
        for( i = 0;i< 10; i++){
            write_characteristic(IDX_CHAR_VAL_A,converted_address,(void*)fake_message,
                                6,gatt_if,conn_id);
        }
        uint8_t buffer[6] = {0};
        int16_t bytes_read;
        for(i = 0; i<10; i++){
            bytes_read = read_characteristic(IDX_CHAR_VAL_A,converted_address,(void*)buffer,6,
                                gatt_if,conn_id);
        }


    }

    void Slave::_print_mac_address(uint8_t* address){
        uint8_t SIZE = 6;
        int i;
        for(i = 0; i<SIZE;i++){
           ESP_LOGE("CLIENT","Byte[%d]: %x",i,address[i]);
        }
       
    }

    void Slave::print_status(){
        std::cout<<"Printing slave status:"<<std::endl;
        
        std::cout <<"Device mac address: "<<this->address<<std::endl;
        std::cout<<"Device connection id: "<<this->device_conn_id<<std::endl;
        std::cout<<"Device connected to internet: "<<this->connected_to_internet<<std::endl;
        std::cout<<"Device is esp: "<<this->esp<<std::endl;
        std::cout<<"Device gatt if: "<<this->device_gatt_if<<std::endl;

        return;


    }


    




}
