#include "master.hpp"
#include <iostream>



namespace bemesh{
    




    Master::Master():connected_clients(),neighbours(){
        
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

    dev_addr_t Master::get_dev_addr(){
        return address;
    }

    void Master::set_dev_addr(dev_addr_t dev_addr){
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


    

    void Master::add_routing_table_entry(dev_addr_t target_addr, dev_addr_t hop_addr,
                            uint8_t num_hops, uint8_t t_flags)
    {
        if(router)
            router->add(target_addr,hop_addr, num_hops,t_flags);                    
    }

    void Master::remove_routing_table_entry(dev_addr_t target_addr){
        if(router)
            router->remove(target_addr);
    }

    void Master::add_routing_table_entry(routing_params_t& routing_params){
        if(router)
            router->add(routing_params);
    }

    dev_addr_t& Master::get_next_hop(dev_addr_t addr){
        return router->nextHop(addr);
    }

    void Master::add_connected_client(uint8_t* new_address){
        //Implement a contains check
        if(!contains_mac(connected_clients,new_address,MAC_ADDRESS_SIZE) && new_address){
            //std::cout<<"Adding"<<std::endl;
            connected_clients.push_back(new_address);
        }
    }

    void Master::remove_connected_client(uint8_t* address ){
        //Implement a contains check.
        if(contains_mac(connected_clients,address,MAC_ADDRESS_SIZE) && address){
            //std::cout<<"Removing"<<std::endl;
            connected_clients.remove(address);
        }
    }

    std::list<uint8_t*> Master::get_connected_clients(){
        std::list<uint8_t*> ret(connected_clients);
        return ret;
    }

    std::list<connected_server_params_t> Master::get_neighbours(){
        std::list<connected_server_params_t> ret(neighbours);
        return ret;
    }


    void Master::add_neighbour(connected_server_params_t new_server){
        uint8_t id = new_server.server_id;
        if(!contains_server(neighbours,id)){
            ESP_LOGE(GATTS_TAG,"Adding a neighbour: %d, %d, %d",new_server.server_id,new_server.gatt_if, new_server.conn_id);
            
            neighbours.push_back(new_server);
        }
    }

    void Master::remove_neighbour(connected_server_params_t server){
        uint8_t id = server.server_id;
        if(contains_server(neighbours, id)){
            std::cout<<"Removing a neighbour"<<std::endl;
            neighbours.remove(server);
        }
    }

    dev_addr_t& Master::get_router_dev_addr(){
        return router->addr();
    }

    std::vector<routing_update_t> Master::get_routing_updates(){
        if(router != NULL)
            return router->getRoutingUpdates();
        std::vector<routing_update_t> vec;
        return vec;
    }

    uint16_t Master::get_internal_client_gatt_if(){
        return internal_client_gatt_if;
    }

    void Master::set_internal_client_gatt_if(uint16_t internal_gatt_if){
        internal_client_gatt_if = internal_gatt_if;
    }

    uint8_t Master::get_internal_client_conn_id(){
        return internal_client_conn_id;
    }

    void Master::set_internal_client_conn_id(uint8_t conn_id){
        internal_client_conn_id = conn_id;
    }

    uint16_t* Master::get_master_buffer_extra_args(){
        return master_message_extra_args;
    }
    
    void Master::ping_reception_callback(MessageHeader* header_t,void * args){
        //ESP_LOGE(GATTS_TAG,"In ping reception callback");
        //Notify to all clients of the ping message.
       

        uint8_t pong_flag;
        RoutingPingMessage* rt_ping_message = (RoutingPingMessage*)header_t;
        

        ESP_LOGE(GATTS_TAG,"In ping reception callback: dest_address is");
        int j;
        uint8_t address_[MAC_ADDRESS_SIZE];
        dev_addr_t addr = rt_ping_message->destination();
        for(j = 0; j<MAC_ADDRESS_SIZE;++j){
            address_[j] = rt_ping_message->destination()[j];
        }
        esp_log_buffer_hex(GATTS_TAG,address_,MAC_ADDRESS_SIZE);

        ESP_LOGE(GATTS_TAG,"In ping reception callback: src_address is");

        for(j = 0; j<MAC_ADDRESS_SIZE;++j){
            address_[j] = rt_ping_message->source()[j];
        }
        esp_log_buffer_hex(GATTS_TAG,address_,MAC_ADDRESS_SIZE);

       
        if(rt_ping_message->pong_flag() == PONG_FLAG_VALUE)
            pong_flag = PONG_FLAG_VALUE;
        else
            pong_flag = PING_FLAG_VALUE;
        uint8_t * server_connids = get_server_connids();
        int i;
        for(i = 0; i<TOTAL_NUMBER_LIMIT; ++i){
			ESP_LOGE(GATTS_TAG,"conn_id is assigned?: %d ",server_connids[i]);
            if(1){
                if(1){
                    uint8_t * client_address = get_connid_MAC(i);
                    dev_addr_t cl_addr = _build_dev_addr(client_address);
                    RoutingPingMessage client_ping_message(cl_addr,rt_ping_message->source(),pong_flag);
                    uint8_t notify = NOTIFY_YES;

                    //ESP_LOGE(GATTS_TAG,"In ping reception callback: beginning to parse all arguments");

                    uint16_t* ptr = (uint16_t*) args;
                    *ptr = 0;
                    uint8_t * _ptr = (uint8_t*)args;
                    _ptr = (uint8_t*) (_ptr + sizeof(uint16_t));
                    *_ptr = i;
                    _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
                    *_ptr = 0;
                    _ptr = (uint8_t*)(_ptr + sizeof(uint8_t*));
                    *_ptr = notify;

                    //ESP_LOGE(GATTS_TAG,"In ping reception callback: ended to parse all arguments");

                    ESP_LOGE(GATTS_TAG,"Pinging client: %d ",i);

                    ErrStatus write_ret = mes_handler.send((MessageHeader*)&client_ping_message);
                    if(write_ret != Success){
                        ESP_LOGE(GATTS_TAG,"Error in sending the message to notify the client: %d ",i);
                    }
                    else{
                        mes_handler.handle();
                    }


                    }
                }
            else{
                ESP_LOGE(GATTS_TAG,"Connids is zero");
            }
         }
        

        //Ping all neighbour servers
        for(auto it = neighbours.begin(); it != neighbours.end(); ++it){
            connected_server_params_t server = *it;
            uint16_t  gatt_if = server.gatt_if;
            uint8_t conn_id = server.conn_id;
            uint8_t server_id = server.server_id;
            uint8_t notify = NOTIFY_NO;

            dev_addr_t dest_addr = server.server_mac_address;

            RoutingPingMessage new_routing_ping_message(dest_addr,rt_ping_message->source(),pong_flag);

            ESP_LOGE(GATTS_TAG,"In ping reception callback: beginning to parse all arguments");

            uint16_t* ptr = (uint16_t*) args;
            *ptr = gatt_if;
            uint8_t * _ptr = (uint8_t*) args;
            _ptr = (uint8_t*) (_ptr + sizeof(uint16_t));
            *_ptr = conn_id;
            _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
            *_ptr = server_id;
            _ptr = (uint8_t*)(_ptr + sizeof(uint8_t*));
            *_ptr = notify;

            ESP_LOGE(GATTS_TAG,"In ping reception callback: ended to parse all arguments");
            
            ESP_LOGE(GATTS_TAG, "Sending ping to the neighbour: ");
            uint8_t _addr[MAC_ADDRESS_SIZE];
            int i;
            for(i =0; i<MAC_ADDRESS_SIZE; i++){
                _addr[i] = dest_addr[i];
            }
            esp_log_buffer_hex(GATTS_TAG,_addr,MAC_ADDRESS_SIZE);
            if(server_id == SERVER_S1 || server_id == SERVER_S2 || server_id == SERVER_S3){
                ErrStatus status = mes_handler.send((MessageHeader*)&new_routing_ping_message);
                if(status != Success){
                    ESP_LOGE(GATTS_TAG,"Error in sending ping to server: %d",server_id);
                    esp_log_buffer_hex(GATTS_TAG,_addr,MAC_ADDRESS_SIZE);
                }
                else{
                    mes_handler.handle();
                }
            }

        }
        
        
        


        return;
    }

    void Master::routing_discovery_request_reception_callback(MessageHeader* header_t,
                            void* args)
    {
        ESP_LOGE(GATTS_TAG, "Someone wants to discover my routing table and I prepare a routing discovery response");
        //I must connect to the server that has requested the routing table and then exchange with it.
		wants_to_discover = false;
        wants_to_send_routing_table = true;

		uint8_t mac[MAC_LEN]; // mac address of the device sending the discovery req
		memcpy(mac, header_t->destination().data(), sizeof(uint8_t) * MAC_LEN);
		ESP_LOGE(GATTS_TAG, "data isssssssssssssss:");
		esp_log_buffer_hex(GATTS_TAG, header_t->destination().data(), MAC_LEN);
		uint8_t conn_id = get_MAC_connid(mac);
		
		
		//Prepare the routing discovery response message in the callback.
		send_routing_table(get_my_MAC(), mac, get_gatt_if(), conn_id, 0);
		
		/*
		uint8_t status = master_instance->is_active(conn_id); 
		ESP_LOGE(GATTS_TAG, "Active? %d, conn_id is %d", status, conn_id);
		switch(status) {
		case 1:
			// I'm active, using write char
			ESP_LOGE(GATTS_TAG, "I'm Active!!");
			break;
		case 0:
			// I'm passive, using notification
			ESP_LOGE(GATTS_TAG, "I'm Passive!!");
			send_routing_table(get_my_MAC(), mac, get_gatt_if(), conn_id, 0); // last argument, server_id, is a bit doubtful
			break;
		default:
			ESP_LOGE(GATTS_TAG, "Who am I?");
			break;
		}
		*/
		
        return;                        
    }

    void Master::routing_discovery_response_reception_callback(MessageHeader* header_t,
                            void* args)
    {
        ESP_LOGE(GATTS_TAG, "Received a discovery response message");
        int i;
        RoutingDiscoveryResponse* res = (RoutingDiscoveryResponse*) header_t;
        for(i = 0; i<res->entries(); i++){
            routing_params_t tmp = res->payload()[i];

            //We update the number of hops.
            tmp.num_hops = tmp.num_hops +1;
            //We add the received routing table entry.
            add_routing_table_entry(tmp);
        }

        return;

    }

    void Master::routing_update_reception_callback(MessageHeader* header_t, void* args)
    {
        ESP_LOGE(GATTS_TAG, "In routing update reception callback");

        //First take the updates and merge.
        RoutingUpdateMessage* routing_updates = (RoutingUpdateMessage*)header_t;
        std::array<routing_update_t,ROUTING_UPDATE_ENTRIES_MAX> updates = routing_updates->payload();
        dev_addr_t sender = routing_updates->source();
        std::vector<routing_update_t> vec_updates(routing_updates->entries());
        ESP_LOGE(GATTS_TAG, "Before std::copy_n");
        std::copy_n(updates.begin(),routing_updates->entries(),vec_updates.begin());

        ESP_LOGE(GATTS_TAG,"Merging the updates calling router->mergeUpdates");
        std::size_t merge_res = router->mergeUpdates(vec_updates,sender);
        ESP_LOGE(GATTS_TAG,"Merge result is (number of entries updated): %d",merge_res);


        //Then propagate the updates until a server can't propagate no longer.
        send_routing_update(updates,routing_updates->entries());

        return;
    }

    void master_reception_callback(MessageHeader* header_t, void* args)
    {
        if(master_instance == NULL)
            return;
        
        switch(header_t->id()){
            case ROUTING_DISCOVERY_REQ_ID:{
                if(master_instance)
                    master_instance->routing_discovery_request_reception_callback(header_t,args);
                break;
            }
            case ROUTING_DISCOVERY_RES_ID:{
                if(master_instance)
                    master_instance->routing_discovery_response_reception_callback(header_t,args);
                break;
            }
            case ROUTING_UPDATE_ID:{
                if(master_instance)
                    master_instance->routing_update_reception_callback(header_t,args);
                break;
            }
            case ROUTING_PING_ID:{
                if(master_instance)
                    master_instance->ping_reception_callback(header_t,args);
                break;
            }
            default:{
                ESP_LOGE(GATTS_TAG,"Unknown message");
                break;
            }
        }
        return;
    }

    void Master::ping_transmission_callback(uint8_t* buffer,uint8_t size, MessageHeader* header_t,
                                    void* args)
    {
        //ESP_LOGE(GATTS_TAG,"In ping transmission callback: beginning to parse all arguments");

        uint16_t * ptr = (uint16_t*) args;
        uint16_t gatt_if = *ptr;

        //We move two bytes forward in the args buffer.
        uint8_t* _ptr = (uint8_t*)args;
        _ptr = (uint8_t*)(_ptr + sizeof(uint16_t));
        uint8_t conn_id = *_ptr;
        _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
        uint8_t server_id = *_ptr;
        _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
        uint8_t notify = *_ptr;

        //ESP_LOGE(GATTS_TAG,"In ping transmission callback: ended to parse all arguments");

        uint8_t characteristic = IDX_CHAR_VAL_A;

        if(notify == NOTIFY_NO){
            write_policy_t policy = Standard;
            ErrStatus write_ret = write_characteristic(characteristic,buffer,size,gatt_if,conn_id,
                                                        policy);
            if(write_ret != Success){
                ESP_LOGE(GATTS_TAG,"In ping transmission callback: Error in writing the characteristic: %d",characteristic);
            }
        }
        else{
            uint8_t notification_ret = send_notification(conn_id,characteristic,buffer,size);
                                                    
            if(notification_ret){
                ESP_LOGE(GATTS_TAG,"In ping transmission callback: Error in notifying the client: %d on the characteristic: %d",conn_id,characteristic);
            }
        }



        return;
    }


    void Master::routing_discovery_request_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args)
    {
        ESP_LOGE(GATTS_TAG,"In routing discovery request transmission callback");
       
        uint8_t characteristic = IDX_CHAR_VAL_B;
        //Writing the packet to the characteristic
        write_policy_t policy = Standard;
        write_characteristic(characteristic,buffer,size,internal_client_gatt_if,
                                    internal_client_conn_id,policy);
                
        return;
    }


    //Take the serialized message contained in buffer, parse the arguments contained in args,
    //then write to the characteristic.
    void Master::routing_discovery_response_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args)
    {
        ESP_LOGE(GATTS_TAG,"In routing discovery response transmission callback");
        /*
        //The order is: gatt_if, conn_id, server_id.
        uint16_t* ptr = (uint16_t*) args;
        uint16_t gatt_if = *ptr;
        uint8_t * _ptr = (uint8_t*)args;
        _ptr = (uint8_t*) (_ptr + sizeof(uint16_t));
        uint8_t conn_id = *(_ptr);
        _ptr =(uint8_t*) (_ptr + sizeof(uint8_t));
        uint8_t server_id = *(_ptr);
        */
        
        uint8_t mac[MAC_LEN];
		memcpy(mac, header_t->source().data(), sizeof(uint8_t) * MAC_LEN);
		uint8_t conn_id = get_MAC_connid(mac);
        uint16_t gatt_if = get_gatt_if();
        
        // Do i really care about server_id? 0 for the moment :)
        esp_log_buffer_hex(GATTS_TAG, mac, MAC_LEN);
        ESP_LOGE(GATTS_TAG,"Parsed message arguments: gatt_if: %d, conn_id: %d, server_id: %d",
                        gatt_if, conn_id, 0);

        uint8_t characteristic = IDX_CHAR_VAL_B;

        //Write to some characteristic
        

        uint8_t status = master_instance->is_active(conn_id); 
		ESP_LOGE(GATTS_TAG, "Active? %d, conn_id is %d", status, conn_id);
		switch(status) {
		case 1: {
			// I'm active, using write char
			ESP_LOGE(GATTS_TAG, "I'm Active!!");
			write_policy_t policy = Standard;
			ErrStatus ret_val = write_characteristic(characteristic,buffer,size,gatt_if,conn_id,policy);
			if(ret_val){
				ESP_LOGE(GATTS_TAG,"In routing_discovery_response_transmission_callback: error in sending the: %d characteristic. Error status: %d",
                                characteristic,ret_val);
			}
			break;
		}
		case 0: {
			// I'm passive, using notification
			ESP_LOGE(GATTS_TAG, "I'm Passive!!");
			uint8_t notification_ret = send_notification(conn_id,characteristic,buffer,size);
			if(notification_ret){
                ESP_LOGE(GATTS_TAG,"In routing_discovery_response_transmission_callback: Error in notifying the client: %d on the characteristic: %d",conn_id,characteristic);
            }
			break;
		}
		default:
			ESP_LOGE(GATTS_TAG, "Who am I?");
			break;
		}
        
        
        
        
		
        return;
    }

    void Master::routing_update_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args)
    {
        ESP_LOGE(GATTS_TAG,"In routing update transmission callback");

        
        
        ESP_LOGE(GATTS_TAG,"In routing update transmission callback. Beginning to parse the arguments");
        //First: take the arguments from the args buffer.
        uint16_t* ptr = (uint16_t*) args;
        uint16_t gatt_if = *ptr;
        uint8_t * _ptr = (uint8_t*) args;
        _ptr = (uint8_t*)(_ptr + sizeof(uint16_t));
        uint8_t conn_id = *_ptr;
        _ptr = (uint8_t*)(_ptr + sizeof(uint8_t));
        uint8_t server_id = *_ptr;
        ESP_LOGE(GATTS_TAG,"In routing update transmission callback. Ended to parse the arguments");
       
       
        uint8_t characteristic = IDX_CHAR_VAL_B;
        write_policy_t policy = Standard;
        write_characteristic(characteristic, buffer,size,gatt_if,conn_id,policy);
        return;
    }
                                    
    void master_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args)
    {
        if(master_instance == NULL)
            return;
        switch(header_t->id()){
            case ROUTING_DISCOVERY_REQ_ID:{
                if(master_instance)
                    master_instance->routing_discovery_request_transmission_callback(buffer,size,header_t,args);
                break;
            }
            case ROUTING_DISCOVERY_RES_ID:{
                if(master_instance)
                    master_instance->routing_discovery_response_transmission_callback(buffer,size,header_t,args);
                break;
            }
            case ROUTING_UPDATE_ID:{
                if(master_instance)
                    master_instance->routing_update_transmission_callback(buffer,size,header_t,args);
                break;
            }
            case ROUTING_PING_ID:{
                if(master_instance)
                    master_instance->ping_transmission_callback(buffer,size,header_t,args);
                break;
            }
            default:{
                ESP_LOGE(GATTS_TAG,"Unknown message for transmission");
                break;
            }
        }
        return;                     
    }

    ErrStatus Master::send_routing_update(std::array<routing_update_t,ROUTING_UPDATE_ENTRIES_MAX> updates,
                                        uint8_t size)
    {
        
        dev_addr_t src_addr = get_router_dev_addr();
        ESP_LOGE(GATTS_TAG,"Preparing to send the update packets to all neighbours");
        for(auto it = neighbours.begin(); it != neighbours.end(); ++it){
            connected_server_params_t server = *it;
            //Retrieve the mac address from the data structure
            dev_addr_t dest_addr = server.server_mac_address;

            //Just for print status (to be commented sonner or later).
            uint8_t addr[MAC_ADDRESS_SIZE];
            int i;
            for(i = 0; i<MAC_ADDRESS_SIZE; ++i){
                addr[i] = dest_addr[i];
            }

            ESP_LOGE(GATTS_TAG,"Sending to the neighbour: ");
            esp_log_buffer_hex(GATTS_TAG, addr,MAC_ADDRESS_SIZE);
            RoutingUpdateMessage routing_update_message(src_addr,dest_addr,updates,ROUTING_UPDATE_ENTRIES_MAX);
            ESP_LOGE(GATTS_TAG,"Ended preparing the message: now I transmit");
            //Before sending we have to set the arguments for the transmission callback
            
            //Push the server gatt_if (uint16_t)
            uint16_t* args = (uint16_t*)master_message_extra_args;
            *args = server.gatt_if;
            uint8_t* _args = (uint8_t*)(args + sizeof(uint16_t));
            //Then the corresponding server conn_id
            *_args = server.conn_id;
            _args = (uint8_t*)(_args + sizeof(uint8_t));
            //Then the server_id
            *_args = server.server_id;
            
            
            //Send the message.
            ESP_LOGE(GATTS_TAG,"Before send");
            ErrStatus ret_val = get_message_handler()->send((MessageHeader*)&routing_update_message);
            if(ret_val != Success){
                ESP_LOGE(GATTS_TAG,"Error in sending the routing updates to: %d",server.server_id);
            }
            get_message_handler()->handle();
        }
        ESP_LOGE(GATTS_TAG,"Ended updating the neighbours");

        //Then flush the output stream. 
        //Need to pass arguments to MessageHandler::handle for this message.

        return Success;

    }

    ErrStatus Master::send_routing_update(){
        std::vector<routing_update_t> r_updates = router->getRoutingUpdates();
        std::size_t num_updates = r_updates.size();
        std::array<routing_update_t,ROUTING_UPDATE_ENTRIES_MAX> arr_updates;
        std::copy_n(r_updates.begin(),num_updates,arr_updates.begin());
        
        dev_addr_t src_addr = get_router_dev_addr();
        //Send the routing updates to all neighbours.
        
        ESP_LOGE(GATTS_TAG,"Preparing to send the update packets to all neighbours");
        for(auto it = neighbours.begin(); it != neighbours.end(); ++it){
            connected_server_params_t server = *it;
            //Retrieve the mac address from the data structure
            dev_addr_t dest_addr = server.server_mac_address;

            uint8_t addr[MAC_ADDRESS_SIZE];
            int i;
            for(i = 0; i<MAC_ADDRESS_SIZE; ++i){
                addr[i] = dest_addr[i];
            }


            ESP_LOGE(GATTS_TAG,"Sending to the neighbour: ");
            esp_log_buffer_hex(GATTS_TAG, addr,MAC_ADDRESS_SIZE);
            RoutingUpdateMessage routing_update_message(src_addr,dest_addr,arr_updates,num_updates);
            
            //Before sending we have to set the arguments for the transmission callback
            
            //Push the server gatt_if (uint16_t)
            uint16_t* args = (uint16_t*)master_message_extra_args;
            *args = server.gatt_if;
            uint8_t* _args = (uint8_t*)(args + sizeof(uint16_t));
            //Then the corresponding server conn_id
            *_args = server.conn_id;
            _args = (uint8_t*)(_args + sizeof(uint8_t));
            //Then the server_id
            *_args = server.server_id;
            
            
            //Send the message.
            ErrStatus ret_val = get_message_handler()->send((MessageHeader*)&routing_update_message);
            if(ret_val != Success){
                ESP_LOGE(GATTS_TAG,"Error in sending the routing updates to: %d",server.server_id);
            }
            get_message_handler()->handle();
        }
        ESP_LOGE(GATTS_TAG,"Ended updating the neighbours");

        //Then flush the output stream. 
        //Need to pass arguments to MessageHandler::handle for this message.

        return Success;


    }
                        

    //Master object main task. Used to test functions and primitives.
    void Master::start(){

        //Can't start if master is not allocated.
        if(master_instance == NULL)
            return;

        //For now there will only be esp.They won't send data to internet.
        set_esp(true);
        set_connected_to_internet(false);

        uint16_t  gatt_if = get_gatt_if();
        uint8_t* mac_address = get_my_MAC();
        uint8_t conn_id = get_client_connid();


        dev_addr_t addr;
        if(mac_address != NULL){
            addr = _build_dev_addr(mac_address);
            //Router object allocation.
            router = new Router(addr);
        }
        
        set_device_gatt_if(gatt_if);
        set_device_connection_id(conn_id);
        set_dev_addr(addr);
        
        ErrStatus ret;
        //Buffer to send/receive messages.
        ret =  mes_handler.installTxBuffer(master_tx_buffer);
        assert(ret == Success);
        ret = mes_handler.installTxCb(&master_transmission_callback);
        assert(ret == Success);
        //Install reception callback for messages. Extra arguments to be added.
        ret = mes_handler.installTxOps(ROUTING_DISCOVERY_REQ_ID,master_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installTxOps(ROUTING_DISCOVERY_RES_ID,master_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installTxOps(ROUTING_UPDATE_ID,master_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installTxOps(ROUTING_PING_ID,master_message_extra_args);
        assert(ret == Success);

        //Passiamo il buffer degli argomenti anche a installOps. Possibile fonte di errori.
        ret = mes_handler.installOps(ROUTING_DISCOVERY_REQ_ID,&master_reception_callback,master_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installOps(ROUTING_DISCOVERY_RES_ID,&master_reception_callback,master_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installOps(ROUTING_UPDATE_ID,&master_reception_callback,master_message_extra_args);
        assert(ret == Success);
        ret = mes_handler.installOps(ROUTING_PING_ID,&master_reception_callback,master_message_extra_args);
        assert(ret == Success);
        
        ESP_LOGE(GATTS_TAG,"Finished installing all things");

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




    ErrStatus Master::write_characteristic(uint8_t characteristic, uint8_t* buffer,
                                        uint16_t buffer_size, uint16_t gatts_if,
                                        uint8_t conn_id, write_policy_t policy)
    {
        if(buffer == NULL)
            return WrongPayload;
        
        if(characteristic == IDX_CHAR_VAL_A || characteristic == IDX_CHAR_VAL_B ||
            characteristic == IDX_CHAR_VAL_C )
        {
            
            /*
            task_param_write_t write_params;
            write_params.conn_id = conn_id;
            write_params.gatt_if = gatts_if;
            write_params.characteristic = characteristic;
            write_params.buffer = buffer;
            write_params.buffer_size = buffer_size;
            write_params.policy = policy;
            */

            //std::cout<<"I'm about to write: "<<"conn_id: "<<conn_id<<"gatt_if: "<<gatts_if;
            //std::cout<<"charact: "<<characteristic<<"data[0]: "<<buffer[0]<<"buffer_size: "<<buffer_size<<std::endl;
            //Spara un task per scrivere su una caratteristica.
            task_param_write_t* write_params = new task_param_write_t;
            write_params->conn_id = conn_id;
            write_params->gatt_if = gatts_if;
            write_params->characteristic = characteristic;
            write_params->buffer = buffer;
            write_params->buffer_size = buffer_size;
            write_params->policy = policy;
            ESP_LOGE(GATTC_TAG, "BUFFER THAT INT_CLIENT IS GOING TO WRITE IS:");
			esp_log_buffer_hex(GATTC_TAG, write_params->buffer, write_params->buffer_size);
            
            xTaskCreate(write_characteristic_task,"write_server",WRITE_TASK_STACK_SIZE,write_params,TASK_PRIORITY,NULL);
            
            return Success;
        }        
        
        else
        {
            return GenericError;
        }


    }

    void Master::shutdown(){
        if(master_instance != NULL){
            delete master_instance->get_router();
            delete master_instance;
        }
    }
    
    /*
        Private functions
    */
    

    //Applicazione che mette in atto il processo di sincronizzazione delle routing table.
    //Interfaccia che permette di inviare i comandi.
    //Nodi che forwardano la routing table.

    //Function that updates the client connected table.
    void Master::update_master_macs(uint8_t* address,uint16_t gatt_if,uint8_t conn_id,
                                    uint8_t server_id, uint8_t flag){
        if(flag == UPDATE_ADD_CLIENT){    
            //std::cout<<"Updating client table"<<std::endl;
            if(address){
                add_connected_client(address);
                //std::cout<<"New address is:"<<std::endl;
                //_print_mac_address(address);
                dev_addr_t addr = _build_dev_addr(address);
                uint8_t hops = 1;
                uint8_t flags = 0;
                dev_addr_t my_addr = get_router_dev_addr();
                add_routing_table_entry(addr,my_addr,hops,flags);
                ESP_LOGE(GATTS_TAG,"Added an entry to the routing table: ");
                //std::cout<<"Added an entry to the routing table: "<<std::endl;
                
                ESP_LOGE(GATTS_TAG,"Sending routing updates to the neighbour list");
                send_routing_update();

                return;
            } 
        }
        else if(flag == UPDATE_REMOVE_CLIENT){
            //std::cout<<"Updating client table"<<std::endl;
            if(address){
                remove_connected_client(address);
                //std::cout<<"Old address is:"<<std::endl;
                //_print_mac_address(address);
                dev_addr_t addr = _build_dev_addr(address);
                remove_routing_table_entry(addr);
                ESP_LOGE(GATTS_TAG,"Removed an entry to the routing table: ");
                //std::cout<<"Removed an entry to the routing table: "<<std::endl;

                ESP_LOGE(GATTS_TAG,"Sending routing updates to the neighbour list");
                send_routing_update();
                return;
            }       
        }
        else if(flag == UPDATE_ADD_SERVER){
            //std::cout<<"Updating server table"<<std::endl;
            if(address){
                dev_addr_t addr = _build_dev_addr(address);
                connected_server_params_t new_server(server_id,gatt_if,conn_id,addr);
                add_neighbour(new_server);
                //ESP_LOGE(GATTS_TAG,"Adding a new neighbour ");

                //std::cout<<"New address is: "<<std::endl;
                /*
                dev_addr_t addr = _build_dev_addr(address);
                uint8_t num_hops = 1;
                uint8_t flags = 0;
                dev_addr_t my_addr = get_router_dev_addr();
                add_routing_table_entry(addr,my_addr,num_hops,flags);
                */

            }
        }
        return;
    }

    ErrStatus Master::send_routing_table(uint8_t* src, uint8_t* dst, uint16_t gatt_if,
                                        uint8_t conn_id,uint8_t server_id)
    {
        dev_addr_t src_address = _build_dev_addr(src);
        dev_addr_t dst_address = _build_dev_addr(dst);
        
        //Passing arguments to the message hanlder.
        ESP_LOGE(GATTS_TAG,"Begin parsing arguments");
        uint16_t* args = (uint16_t*)master_message_extra_args;
        *args = gatt_if;
        uint8_t* _args = (uint8_t*) (args + sizeof(uint16_t));
        *_args = conn_id;
        _args = (uint8_t*)(_args+ sizeof(uint8_t));
        *_args = server_id;
        ESP_LOGE(GATTS_TAG,"Ended parsing arguments");

        std::vector<routing_params_t>rtable = master_instance->get_router()->getRoutingTable();
        std::cout<<"Extracting routing table"<<std::endl;

        int table_entries = rtable.size();
        std::array<routing_params_t,ROUTING_DISCOVERY_RES_ENTRIES_MAX>rtable_array;

        //Copy the elements of the vector into the array.
        std::copy_n(rtable.begin(),table_entries,rtable_array.begin());
        std::cout<<"Routing table copied and array formed"<<std::endl;
        RoutingDiscoveryResponse routing_discovery_res_message(src_address,dst_address,
                                    rtable_array,table_entries);
        ErrStatus ret_val = master_instance->get_message_handler()->send((MessageHeader*)&routing_discovery_res_message);
        

        
        std::cout<<"RoutingDiscoveryResponseMessage sent"<<std::endl;
        master_instance->get_message_handler()->handle(); // qui chiamo routing_discovery_response_transmission callback
        std::cout<<"Message handled"<<std::endl;
        //And see what happens.
        return ret_val;
    }

    void Master::parse_message_receive(uint8_t* packet, uint16_t size){
        std::size_t bytes_read = mes_handler.read(packet);
        if(bytes_read != size){
            ESP_LOGE(GATTS_TAG,"In Master::parse_message_receive. Expected to read: %d bytes but %d actually read",size, bytes_read);
        }

        mes_handler.handle();
        return;
    }

    void Master::parse_message_send(MessageHeader* handler_t){
        ErrStatus ret = mes_handler.send(handler_t);
        if(ret != Success){
            ESP_LOGE(GATTS_TAG,"In Master::parse_message_send. Error. status: %d",ret);    
        }
        mes_handler.handle();
        return;
    }
    



    uint8_t Master::send_notification(uint8_t conn_id,uint8_t characteristic,uint8_t* data,
                                    uint8_t data_size)
    {
        ESP_LOGE(GATTS_TAG,"In send notification conn_id: %d characteristic: %d",
                            conn_id, characteristic);
        uint8_t notification_ret;
        
        notification_ret = notify_client(conn_id,characteristic,data,data_size);
        return notification_ret;
    }
	
	
	uint8_t Master::is_active(uint8_t conn_id){
        return active[conn_id];
    }
    
    void Master::set_active(uint8_t conn_id){
        active[conn_id] = 1;
        return;
    }
    
    void Master::set_passive(uint8_t conn_id){
        active[conn_id] = 0;
        return;
    }
}

