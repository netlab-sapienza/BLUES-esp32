#include "slave.hpp"
int bemesh_errno=0;
//TO-DO. complete stuff.

namespace bemesh{
    Slave::Slave():ping_response_list(){

        //Toglierle(?)
        esp = true;
        connected_to_internet = false;

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

    uint8_t* Slave::get_slave_message_extra_args(){
        return slave_message_extra_args;
    }

    void Slave::add_routing_table_entry(dev_addr_t target_addr, dev_addr_t hop_addr,
                            uint8_t num_hops, uint8_t t_flags)
    {
        router->add(target_addr,hop_addr, num_hops,t_flags);                    
    }

    void Slave::remove_routing_table_entry(dev_addr_t target_addr){
        router->remove(target_addr);
    }

    dev_addr_t& Slave::get_next_hop(dev_addr_t addr){
        return router->nextHop(addr);
    }

    void Slave::add_ping_response(ping_data_t rsp){
        ping_response_list.push_back(rsp);
        return;
    }

    void Slave::remove_ping_response(ping_data_t rsp){
        ping_response_list.remove(rsp);
        return;
    }

    std::list<ping_data_t> Slave::get_ping_response(){
        std::list<ping_data_t> ret(ping_response_list);
        return ret;
    }


    uint8_t* Slave::get_server_dev_addr(){
        return server_dev_addr;
    }

    void Slave::set_server_dev_addr(uint8_t* dev_addr){
        server_dev_addr = dev_addr;
        return;
    }



    void slave_transmission_callback(uint8_t* message, uint8_t size, MessageHeader* header_t,
                                    void* args)
    {
        switch(header_t->id()){
            case ROUTING_UPDATE_ID:{
                if(slave_instance)
                    slave_instance->routing_update_transmission_callback(message,size,header_t, args);
                break;
            }
            case ROUTING_PING_ID:{
                if(slave_instance)
                    slave_instance->ping_transmission_callback(message,size,header_t,args);
            }
            default:{
                break;
            }
        }
        return;                            
    }   

    void Slave::ping_transmission_callback(uint8_t* message, uint8_t size, MessageHeader* header_t,
                                            void * args)
    {
        //Check argument integrity
        if(message == NULL || header_t == NULL)
            return;
        if(size == 0)
            return;


        //ESP_LOGE(GATTC_TAG,"The ping message I'm about to transmit is");
        
       // esp_log_buffer_hex(GATTC_TAG,message,size);
        
        //ESP_LOGE(GATTC_TAG,"In ping transmission callback: we begin to retrieve the arguments");
        //Begin to parse the arguments
        uint16_t* ptr = (uint16_t*) args;
        uint16_t gattc_if = *ptr;
        uint8_t * _ptr = (uint8_t*)args;
        _ptr = (uint8_t*)(_ptr + sizeof(uint16_t));
        uint8_t conn_id = *(_ptr);
        _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
        uint8_t characteristic = *(_ptr);
        //ESP_LOGE(GATTC_TAG,"In ping transmission callback: we ended to retrieve the arguments");
       
        write_policy_t policy  = Standard;
        ErrStatus ret_status = write_characteristic(characteristic,message,size,gattc_if,
                                conn_id, policy);
        if(ret_status != Success){
            ESP_LOGE(GATTC_TAG,"In ping transmission callback: %d",ret_status);
        }
        return;


    }

    void Slave::routing_update_transmission_callback(uint8_t* message,uint8_t size, MessageHeader* header_t,
                                            void* args)
    {
        ESP_LOGE(GATTC_TAG, "In routing update transmission callback");
        return;
    }


    void Slave::ping_reception_callback(MessageHeader* header_t, void* args){
        //ESP_LOGE(GATTC_TAG, "In ping reception callback");

        //ESP_LOGE(GATTC_TAG,"In ping reception callback. Beginning to parse the arguments.");
        uint16_t* ptr = (uint16_t*)args;
        uint16_t gatt_if = *ptr;
        uint8_t* _ptr = (uint8_t*) args;
        _ptr = (uint8_t*)(_ptr + sizeof(uint16_t));
        uint8_t conn_id = *(_ptr);
        _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
        uint8_t characteristic = *(_ptr);
        RoutingPingMessage* routing_ping_message = (RoutingPingMessage*)header_t;

        //ESP_LOGE(GATTC_TAG,"In ping reception callback. Ended to parse all the arguments.");
        
    
        uint8_t _pong_flag = routing_ping_message->pong_flag();

        //ESP_LOGE(GATTC_TAG,"Added a new entry into the ping_response list");
        ping_data_t p_data(routing_ping_message->source(),_pong_flag,conn_id,
                                                gatt_if);
        

        /*uint8_t source_addr[MAC_ADDRESS_SIZE];
        uint8_t my_addr[MAC_ADDRESS_SIZE];
        int i;
        for(i = 0; i< MAC_ADDRESS_SIZE;++i){
            source_addr[i] = routing_ping_message->source()[i];
            my_addr[i] = get_dev_addr()[i];
        }

        ESP_LOGE(GATTC_TAG,"routing_ping_message->source(): byte[0] = %x",routing_ping_message->source()[0]);
        esp_log_buffer_hex(GATTC_TAG,source_addr,MAC_ADDRESS_SIZE);
        ESP_LOGE(GATTC_TAG,"my address");
        esp_log_buffer_hex(GATTC_TAG,my_addr,MAC_ADDRESS_SIZE);
    */



        if(same_addresses(routing_ping_message->source(),_build_dev_addr(get_dev_addr()),MAC_ADDRESS_SIZE)){
            ESP_LOGE(GATTC_TAG,"I received pong from the server. But I sent it");
            return;

        }
        
        else{
            ESP_LOGE(GATTC_TAG, "Adding ping_data_t element to the ping response list");
            add_ping_response(p_data);
        }

        RoutingPingMessage new_ping_message(routing_ping_message->source(),_build_dev_addr(get_dev_addr()),PONG_FLAG_VALUE);
        send_message(get_device_gatt_if(),get_device_connection_id(),NULL,
                        (MessageHeader*)&new_ping_message, characteristic);

        
        


        //Send the pong to the server.
        ping_server(get_device_gatt_if(),get_server_connection_id(),server_dev_addr,_pong_flag);
        return;
    }



    void slave_reception_callback(MessageHeader* header_t, void* args){
        switch(header_t->id()){
            case ROUTING_UPDATE_ID: {
                if(slave_instance)
                    slave_instance->routing_update_reception_callback(header_t,args);
                break;
            }
            case ROUTING_PING_ID:{
                if(slave_instance)
                    slave_instance->ping_reception_callback(header_t,args);
                break;
            }
            default:{
                break;
            }
        }
        return;
    }


    void Slave::routing_update_reception_callback(MessageHeader* header_t, void* args){
        ESP_LOGE(GATTC_TAG,"In routing update reception callback");
        return;
    }

   


    //Implementare, in seguito, un check su conn_id e gattc_if
    uint8_t* Slave::read_characteristic(uint8_t characteristic, uint16_t gattc_if,
                                        uint16_t conn_id)
    {
        if(characteristic == IDX_CHAR_VAL_A || characteristic == IDX_CHAR_VAL_B || 
            characteristic == IDX_CHAR_VAL_C)
        {

            uint8_t* received_bytes=  read_CHR(gattc_if,conn_id,characteristic);
            uint8_t char_len_read = get_CHR_value_len(characteristic);
            ESP_LOGE(GATTC_TAG,"Performed read on characteristic: %d ",characteristic);
            esp_log_buffer_hex(GATTC_TAG,received_bytes,char_len_read);
            return received_bytes;

        }
        else
            return NULL;    

    }


    ErrStatus Slave::send_message(uint16_t gattc_if,uint8_t conn_id,uint8_t* address,MessageHeader* header_t,
                            uint8_t characteristic)
    {
        if(header_t == NULL){
            return WrongPayload;
        }
        else
        {
            //Begin to parse the argument for the message handler. The order in this case is:
            //gatt_if conn_id characteristic
            uint16_t* args = (uint16_t*)slave_message_extra_args;
            *args = gattc_if;
            uint8_t * _args = (uint8_t*) slave_message_extra_args;
            _args = (uint8_t*)(_args + sizeof(uint16_t));
            *_args = conn_id;
            _args = (uint8_t*)(_args + sizeof(uint8_t));
            *_args = characteristic;
            

            //First send the message into the buffer.
            ErrStatus  status = mes_handler.send(header_t);
            if(status != Success){
                ESP_LOGE(GATTC_TAG,"In Slave::send_message. Error in MessageHandler::send");
                return status;
            }

            //Flush the output
            mes_handler.handle();
            return Success;
        }
        //Implementare una sorta di meccanismo di acknowledgement se possibile.
        
            

    }



    ErrStatus Slave::write_characteristic(uint8_t characteristic,uint8_t* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint8_t conn_id, write_policy_t policy)
    {
        if(buffer == NULL)
            return WrongPayload;
        

        if(characteristic == IDX_CHAR_VAL_A || characteristic == IDX_CHAR_VAL_B ||
            characteristic == IDX_CHAR_VAL_C )
        {	/*
            task_param_write_t write_params;
            write_params.conn_id = conn_id;
            write_params.gatt_if = gattc_if;
            write_params.characteristic = characteristic;
            write_params.buffer = buffer;
            write_params.buffer_size = buffer_size;
            write_params.policy = policy;
            //std::cout<<"I'm about to write: "<<"conn_id: "<<conn_id<<"gatt_if: "<<gatts_if;
            //std::cout<<"charact: "<<characteristic<<"data[0]: "<<buffer[0]<<"buffer_size: "<<buffer_size<<std::endl;
            //Spara un task per scrivere su una caratteristica.
            
            ESP_LOGE(GATTC_TAG,"TEST IN THE SLAVE 2: conn_id %d, gatt_if %d", write_params.conn_id, write_params.gatt_if);
            */
            task_param_write_t* write_params = new task_param_write_t;
            write_params->conn_id = conn_id;
            write_params->gatt_if = gattc_if;
            write_params->characteristic = characteristic;
            write_params->buffer = buffer;
            write_params->buffer_size = buffer_size;
            write_params->policy = policy;
            
            xTaskCreate(write_characteristic_task,"write task",WRITE_TASK_STACK_SIZE,(void*)write_params,TASK_PRIORITY,NULL);
            return Success;
        }        
        
        else
        {
            return GenericError;
        }

    }


   


    
    void Slave::shutdown(){
        if(slave_instance != NULL)
            delete slave_instance;
    }

    void Slave::start(){
        if(slave_instance == NULL)
            return;
        
        //Initialize all fields of the class;
        uint16_t gatt_if = get_gatt_if();
        uint8_t* mac_address = get_my_MAC();
        uint8_t conn_id = get_client_connid();
        uint8_t* server_mac_address = get_connid_MAC(conn_id);
        esp_log_buffer_hex(GATTC_TAG,mac_address,MAC_ADDRESS_SIZE);


        set_device_gatt_if(gatt_if);
        set_device_connection_id(conn_id);
        set_dev_addr(mac_address);
        set_server_dev_addr(server_mac_address);

        dev_addr_t converted_address = _build_dev_addr(mac_address);  
        if(mac_address != NULL){
            router = new Router(converted_address);
        }


        //Since this is a client, its next hop is always a server. We add the entry in
        //the routing table.
        dev_addr_t converted_server_mac_address = _build_dev_addr(server_mac_address);
        uint8_t server_hops = 1;
        uint8_t server_flags = 0;
        add_routing_table_entry(converted_address,converted_server_mac_address,server_hops,
                            server_flags);

            




        ErrStatus ret;
        //Buffer to send/receive messages.
        ret =  mes_handler.installTxBuffer(slave_tx_buffer);
        assert(ret == Success);
        ret = mes_handler.installTxCb(&slave_transmission_callback);
        assert(ret == Success);
        //Install reception callback for messages. Extra arguments to be added.
        //ret = mes_handler.installTxOps(ROUTING_DISCOVERY_REQ_ID,slave_message_extra_args);
        //assert(ret == Success);
        //ret = mes_handler.installTxOps(ROUTING_DISCOVERY_RES_ID,slave_message_extra_args);
        //assert(ret == Success);
        ret = mes_handler.installTxOps(ROUTING_UPDATE_ID,slave_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installTxOps(ROUTING_PING_ID,slave_message_extra_args);
        assert(ret == Success);
        

        //ret = mes_handler.installOps(ROUTING_DISCOVERY_REQ_ID,reception_callback,nullptr);
        //assert(ret == Success);
        //ret = mes_handler.installOps(ROUTING_DISCOVERY_RES_ID,&reception_callback,nullptr);
        //assert(ret == Success);

        //Passiamo il buffer degli argomenti anche ad installOps(possibile fonte di bug)
        ret = mes_handler.installOps(ROUTING_UPDATE_ID,&slave_reception_callback,slave_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installOps(ROUTING_PING_ID,&slave_reception_callback,slave_message_extra_args);
        assert(ret == Success);
        
        ESP_LOGE(GATTC_TAG,"Finished installing all things");
        uint8_t characteristic = IDX_CHAR_VAL_A;
        int BUFFER_SIZE = 6;
        uint8_t buffer[BUFFER_SIZE] = {1,2,3,4,5,6};

        //We try to write something.
        ESP_LOGE(GATTC_TAG,"I'm about to write something on the server");
        int i;
        write_policy_t policy = Standard;
        
        //It should be thread safe by the way.
        int old_errno = bemesh_errno;
        ESP_LOGE(GATTC_TAG,"TEST IN THE SLAVE: conn_id %d, gatt_if %d", conn_id, gatt_if);
        //ErrStatus write_ret = write_characteristic(characteristic,buffer,BUFFER_SIZE,gatt_if,conn_id,policy);
        //if(write_ret != Success)
            //ESP_LOGE(GATTC_TAG,"Errore in write characteristic: %d",write_ret);
        if(bemesh_errno == E_WRITE_CHR){
            ESP_LOGE(GATTC_TAG, "Errore nella write: E_WRITE_CHR");
        }
        bemesh_errno = old_errno;



        //Begin testing the ping features
        //ping_server(gatt_if,conn_id,server_mac_address,PING_FLAG_VALUE);

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

    


    ErrStatus Slave::ping_server(uint16_t gatt_if,uint8_t conn_id, uint8_t* mac_address,
                                uint8_t pong_flag){
        
        if(address == NULL)
            return GenericError;

        //Check if the mac address is in the routing table (robustness check).

        //Start pinging the server
        dev_addr_t src_addr = _build_dev_addr(get_dev_addr());
        dev_addr_t src_addr_;
        int j;
        for(j = 0; j<MAC_ADDRESS_SIZE; j++){
            src_address_to_send[j] = src_addr[j];
        }
        dev_addr_t dest_addr = _build_dev_addr(mac_address);


        RoutingPingMessage routing_ping_message(dest_addr,src_addr,pong_flag);
        uint8_t characteristic = IDX_CHAR_VAL_A;
        
        uint16_t* args = (uint16_t*)slave_message_extra_args;
        *args = gatt_if;
        uint8_t * _args = (uint8_t*) slave_message_extra_args;
        _args = (uint8_t*)(_args + sizeof(uint16_t));
        *_args = conn_id;
        _args = (uint8_t*)(_args + sizeof(uint8_t));
        *_args = characteristic;
        //ESP_LOGE(GATTC_TAG,"In ping transmission. src address is: ");
        //esp_log_buffer_hex(GATTC_TAG,get_dev_addr(),MAC_ADDRESS_SIZE);
        //ESP_LOGE(GATTC_TAG,"In ping transmission. dest_addr address is: ");
        //esp_log_buffer_hex(GATTC_TAG,mac_address,MAC_ADDRESS_SIZE);
        //First send the message into the buffer.
        ErrStatus  status = mes_handler.send((MessageHeader*)&routing_ping_message);
        if(status != Success){
            ESP_LOGE(GATTC_TAG,"In Slave::send_message. Error in MessageHandler::send");
            return status;
        }

            //Flush the output
        mes_handler.handle();
        return Success;
        
        //ErrStatus ret = send_message(gatt_if,conn_id,mac_address,(MessageHeader*)&routing_ping_message,characteristic);
        //if(ret != Success){
        //    ESP_LOGE(GATTC_TAG,"Error in sending the ping to the connected server");
        //}
        return Success;


    }


    




}
