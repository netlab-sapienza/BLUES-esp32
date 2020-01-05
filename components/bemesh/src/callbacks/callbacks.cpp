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


    void Callback::server_update_callback( uint8_t* macs,uint8_t flag){
        master_instance->update_master_macs(macs,flag);
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


    void Callback::send_routing_table_callback(uint8_t* src, uint8_t* dst, 
                                        uint16_t gatt_if, uint8_t conn_id)
    {
        dev_addr_t src_address = _build_dev_addr(src);
        dev_addr_t dst_address = _build_dev_addr(dst);

        std::vector<routing_params_t>rtable = master_instance->get_router()->getRoutingTable();
        std::cout<<"Extracting routing table"<<std::endl;

        int table_entries = rtable.size();
        std::array<routing_params_t,ROUTING_DISCOVERY_RES_ENTRIES_MAX>rtable_array;

        //Copy the elements of the vector into the array.
        std::copy_n(rtable.begin(),table_entries,rtable_array.begin());
        std::cout<<"Routing table copied and array formed"<<std::endl;
        RoutingDiscoveryResponse routing_discovery_res_message(src_address,dst_address,
                                    rtable_array,table_entries);
        master_instance->get_message_handler()->send((MessageHeader*)&routing_discovery_res_message);
        std::cout<<"RoutingDiscoveryResponseMessage sent"<<std::endl;
        master_instance->get_message_handler()->handle();
        std::cout<<"Message handled"<<std::endl;
        //And see what happens.

    }

    void Callback::notify_callback(void){
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

        ESP_LOGE(FUNCTOR_TAG,"HO FINITO DI INSTALLARE LE CALLBACKS");
        return;
    }


    

}