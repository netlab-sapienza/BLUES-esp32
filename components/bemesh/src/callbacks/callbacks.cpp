#include "callbacks.hpp"



namespace bemesh{
    Master* master_instance;
    Slave* slave_instance;


    Callback::Callback(){}

    void Callback::init_callback(uint8_t type){
        switch(type){
            case SERVER:{
                master_instance = new Master();
                ESP_LOGE(FUNCTOR_TAG,"OGGETTO MASTER SULL'HEAP");
                
                //Inizializzazione e start del server.
                master_instance->start();

                //Try to find out if there is another server.
                register_internal_client(SERVER_S1);
                return;
            }
            case CLIENT:{
                slave_instance = new Slave();
                ESP_LOGE(FUNCTOR_TAG,"OGGETTO SLAVE SULL'HEAP");
                slave_instance->start();
                return;
            }
            default:{
                ESP_LOGE(FUNCTOR_TAG, "DUNNO WHAT I'M DOING");
            }
        }
    }

    void Callback::send_routing_table_callback(uint8_t* src, uint8_t* dst, uint16_t gatt_if,
                                            uint8_t conn_id)
    {

    }

    void Callback::shutdown_device_callback(uint8_t type){
        switch(type){
            case SERVER:{
                ESP_LOGE(FUNCTOR_TAG,"Shutting down the master object");
                master_instance->shutdown();
            }
            case CLIENT:{
                ESP_LOGE(FUNCTOR_TAG,"Shutting down the slave object");
                slave_instance->shutdown();
            }
        }
    }

    void Callback::server_update_callback( uint8_t* mac,uint8_t flag,uint16_t gatt_if, uint8_t conn_id,
                                           uint8_t server_id){
        master_instance->update_master_macs(mac,gatt_if,conn_id,server_id,flag);
        //Send a routing update to the neighbouring servers.

        //Commented for test reasons.
        //master_instance->prepare_routing_update();
    }

    //Invoked when two servers meet each other.
    void Callback::exchange_routing_table_callback(uint8_t* src_address,uint8_t* dest_address,
                                uint16_t gattc_if,uint8_t conn_id){
        
        //Set the internal client gatt_if.
        master_instance->set_internal_client_gatt_if(gattc_if);
        master_instance->set_internal_client_conn_id(conn_id);
        //::vector<routing_update_t> r_updates = master_instance->get_routing_updates();
        dev_addr_t src_addr = _build_dev_addr(src_address);
        dev_addr_t dest_addr = _build_dev_addr(dest_address);
        //std::cout<<"Address built"<<std::endl;
        RoutingDiscoveryRequest routing_discovery_request_message(src_addr,dest_addr);
        //Serialize message (prepare message)
                           
        master_instance->get_message_handler()->send((MessageHeader*)&routing_discovery_request_message);
        //std::cout<<"Message sent"<<std::endl;
        master_instance->get_message_handler()->handle();
        //std::cout<<"Messange handled"<<std::endl;
        //And see what happens.
        

    }




    void Callback::received_packet_callback(uint8_t* packet,uint16_t size){
        if(!packet )
            return;
        std::cout<<"Received a packet"<<std::endl;
        esp_log_buffer_hex(FUNCTOR_TAG,packet,size);
        //Read the packet.
        master_instance->get_message_handler()->read(packet);
        
        //Then call the handle function.
        master_instance->get_message_handler()->handle();
        //then see what happens.

    }

    void Callback::notify_callback(uint16_t gatt_if,uint8_t conn_id,uint8_t characteristic){
        //Triggered when a client is notified. The client can now read the characteristic
        
        //(Read the characteristic)
        uint8_t* received_bytes =  slave_instance->read_characteristic(characteristic,gatt_if, conn_id);
        //Check if read is complete.
        //Then see what happens.

    }


    //In ASSERT we trust!!
    void Callback::operator ()(){
        uint8_t ret = install_InitCb(init_callback);
        if(ret){
            ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della init_callback");
        }
        assert(ret == 0);
        ret = install_NotifyCb(notify_callback);
        if(ret){
            ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della notify_callback");
        }
        assert(ret == 0);
        ret = install_ServerUpdateCb(server_update_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della server_update_callback"); 
        }
        assert(ret == 0);        
        ret = install_ExchangeRoutingTableCb(exchange_routing_table_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della exchange_routing_table_callback"); 
        }
        assert(ret == 0);
        ret = install_ReceivedPacketCb(received_packet_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della received_packet_callback"); 
        }
        assert(ret == 0);
        
        ret = install_ShutDownCb(shutdown_device_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della shutdown_callback"); 
        }
        assert(ret == 0);



        ESP_LOGE(FUNCTOR_TAG,"HO FINITO DI INSTALLARE LE CALLBACKS");
        return;
    }



    

}