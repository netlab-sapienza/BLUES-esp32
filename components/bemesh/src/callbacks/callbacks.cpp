#include "callbacks.hpp"



namespace bemesh{
    Master* master_instance;
    Slave* slave_instance;
    bool discarded[SCAN_LIMIT] = {false};

    Callback::Callback(){
            int i;
            //Set the discarded devices to false in principle.
            ESP_LOGE(FUNCTOR_TAG,"In Callback ctor!!");
            for(i = 0; i<SCAN_LIMIT; ++i)
                discarded[i] = false;
    }

    void Callback::ssc_active_callback(uint8_t internal_client_id){
        uint8_t BUF_SIZE = 6;
        uint8_t buffer[BUF_SIZE]={1,1,1,1,1,1};
        uint8_t other_buff[BUF_SIZE]={5,5,5,5,5,5};
        write_policy_t policy = Standard;
        if(master_instance){
            ESP_LOGE(GATTS_TAG,"In ssc_active_callback. writing");
            master_instance->write_characteristic(IDX_CHAR_VAL_A,buffer,BUF_SIZE,get_internal_client_gattif(internal_client_id),
                                                get_internal_client_connid(internal_client_id),policy);

            
            
        }
        else if(slave_instance){
            ESP_LOGE(GATTC_TAG,"In ssc_active_callback. Sending a notification");
            slave_instance->write_characteristic(IDX_CHAR_VAL_A,buffer,BUF_SIZE,get_gatt_if(),
                            get_internal_client_connid(internal_client_id),Standard);
        }
        return;
    }

    void Callback::ssc_passive_callback(uint8_t conn_id){
        uint8_t BUF_SIZE = 6;
        uint8_t characteristic = IDX_CHAR_VAL_A;
        uint8_t buffer[BUF_SIZE] = {2,2,2,2,2,2};
        if(master_instance){
            ESP_LOGE(GATTS_TAG,"In ssc_passive_callback. Sending a notification");
            master_instance->send_notification(conn_id,characteristic,buffer,BUF_SIZE);
        }

    }

    void Callback::init_callback(uint8_t type){
        switch(type){
            case SERVER:{
                master_instance = new Master();
                ESP_LOGE(FUNCTOR_TAG,"OGGETTO MASTER SULL'HEAP");
                
                //Inizializzazione e start del server.
                master_instance->start();
                esp_log_buffer_hex(GATTS_TAG,get_my_MAC(),MAC_ADDRESS_SIZE);

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
                                            uint8_t conn_id,uint8_t server_id)
    {
        master_instance->send_routing_table(src,dst,gatt_if,conn_id,server_id);

        //After we send the routing table we unregister the internal client.
        unregister_internal_client(SERVER_S2);
        ESP_LOGE(FUNCTOR_TAG,"Client number: %d unregistered",SERVER_S2);

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



    //It will triggere the correct message callback.
    void Callback::received_packet_callback(uint8_t* packet,uint16_t size){
        if(!packet )
            return;
        std::cout<<"Received a packet"<<std::endl;
        esp_log_buffer_hex(FUNCTOR_TAG,packet,size);
        //Read the packet.
        master_instance->parse_message_receive(packet,size);

    }

    void Callback::notify_callback(uint16_t gatt_if,uint8_t conn_id,uint8_t characteristic){
        //Triggered when a client is notified. The client can now read the characteristic
        
        //(Read the characteristic)
        uint8_t* received_bytes =  slave_instance->read_characteristic(characteristic,gatt_if, conn_id);
        //Check if read is complete.
        //Then see what happens.

    }

    //Choose a server to connect to according to the policy "policy"
    int Callback::choose_server(device* device_list,int device_list_size,uint8_t internal_flag,
                                uint8_t server_id, connection_policy_t policy){
        
        //Check if the buffer exists.
        if(device_list == NULL)
            return -1;

        //Error: illegal device list size.
        if(device_list_size > SCAN_LIMIT)
            return -1;
        
        //Then choose the server according to the policy.
        switch(policy){
            case Minimum_rssi_value_policy: {
                int i;
                uint8_t rssi_min = device_list[0].rssi;
                int server_pos = 0; 
                for( i = 1; i< device_list_size; i++){
                    if(discarded[i] == false){
                        if(device_list[i].rssi < rssi_min){
                            rssi_min = device_list[i].rssi;
                            server_pos = i;
                        }
                    }
                }
                return server_pos;
            }
            case Maximum_rssi_value_policy:{
                int i;
                uint8_t rssi_max = device_list[0].rssi;
                int server_pos = 0;
                for(i = 0; i< device_list_size; ++i){
                    if(discarded[i] == false){
                        if(device_list[i].rssi > rssi_max){
                            rssi_max = device_list[i].rssi;
                            server_pos = i;
                        }
                    }
                }
                return server_pos;
            }
            case Random_policy:{
                break;
            }
            case Fcfs_policy:{
                break;
            }
            default:{
                ESP_LOGE(GATTC_TAG, "Unknown policy value in choosing the server");
                break;
            }
        }
        return -1;
    }

    bool Callback::check_all_discarded(){
        int i;
        bool ret = true;
        for(i = 0; i<SCAN_LIMIT; i++){
            if(discarded[i] == false)
                return false;
        }
        return ret;
    }

    void Callback::reset_discarded(){
        int i;
        for(i = 0; i<SCAN_LIMIT;++i)
            discarded[i] = false;
    }
    int Callback::connect_to_server(device* device_list, int device_list_size,
                                    uint8_t internal_flag, uint8_t server_id,
                                    connection_policy_t policy){
        bool all_discarded = check_all_discarded();
        //ESP_LOGE(GATTC_TAG,"I checked");
        if(all_discarded)
            return -1;
        else{
            int server_pos = choose_server(device_list,device_list_size,internal_flag,server_id,policy);
            uint8_t connection_ret = connectTo(device_list[server_pos],internal_flag,server_id);
            if(connection_ret){
                discarded[server_pos] = true;
                //We recursively look for another server keeping track of the discarded one.
                int conn_ret = connect_to_server(device_list,device_list_size,internal_flag,
                                        server_id,policy);
                return conn_ret;
            }
            else{
                reset_discarded();
                return 0;
            }
        }
    }

    void Callback::endscanning_callback(device* device_list,uint8_t count,
                                        uint8_t internal_flag,uint8_t server_id){
        int i;
        

        //We perform some integrity check.
        if(device_list == NULL)
            return;

        if(count > SCAN_LIMIT){
            ESP_LOGE(GATTC_TAG,"Error in endscanning_callback. Device number: %d exceed the SCAN_LIMIT: %d",
                                count,SCAN_LIMIT);
            return;
        }

        for(i = 0; i<count; i++){
            
            uint8_t* addr = device_list[i].mac;
            uint8_t type = device_list[i].addr_type;
            uint8_t num_clients = device_list[i].clients_num;
            uint8_t signal_strength = device_list[i].rssi;
            ESP_LOGE(GATTC_TAG,"Found this server at the end of scanning: address:");
            esp_log_buffer_hex(GATTC_TAG,addr, MAC_ADDRESS_SIZE);
            ESP_LOGE(GATTC_TAG,"type: %d num_clients: %d signal strength: %d",type,num_clients,signal_strength);
        
        }
        ESP_LOGE(GATTC_TAG, "Out of the loop: we have collected a device list of: %d elements",count);
        connection_policy_t policy = Maximum_rssi_value_policy;
        if (count == 0){
            //If no server is found we become a server ourselves
            becomeServer();
            //ESP_LOGE(GATTC_TAG,"I become a server");
            return;
        }
        else{
            int server_pos = connect_to_server(device_list,count,internal_flag,server_id,policy);
            if(server_pos == -1){
                ESP_LOGE(GATTC_TAG, "Unable to connect to any server");
                return;
            }
            else{
                ESP_LOGE(GATTC_TAG,"Chosen server in pos: %d",server_pos);
                if(CLIENT_FLAG)
                    init_callback(CLIENT);
                
            }
        }
    }

    void Callback::server_lost_callback(void){

        return;
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

        ret = install_SendRoutingTableCb(send_routing_table_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della send_routing_table_callback"); 
        }
        assert(ret == 0);
        
        ret = install_ShutDownCb(shutdown_device_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della shutdown_callback"); 
        }
        assert(ret == 0);

         
        ret = install_EndScanning(endscanning_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della endscanning_callback"); 
        }
        assert(ret == 0);
    
        ret = install_ServerLost(server_lost_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della server_lost_callback"); 
        }
        assert(ret == 0);
        
        ret = install_SSC_Active(ssc_active_callback);      
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della ssc_active_callback"); 
        }
        assert(ret == 0);
        
        ret = install_SSC_Passive(ssc_passive_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della ssc_passive_callback"); 
        }
        assert(ret == 0);
        




        ESP_LOGE(FUNCTOR_TAG,"HO FINITO DI INSTALLARE LE CALLBACKS");
        return;
    }



    

}
