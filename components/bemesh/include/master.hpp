
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"


//
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"
#include <list>





#include "services.hpp"
#include "constant.hpp"
#include "common.hpp"


//Mid-tier.
#include "rtable.hpp"
#include "routing.hpp"
#include "message_handler_v2.hpp"




extern "C"{
    #include "kernel.h"
    #include "gatts_table.h"
}

#include <string>





#define NUM 2

namespace  bemesh{
    
    
    //Reception callback for messages. From here all callbacks will be invoked
    //according to the message_id.
    void master_reception_callback(MessageHeader* header_t, void* args);
    
                
    //Trtansmission callback for messages. From here all callbacks will be invoked
    //according to the message_id.
    void master_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                void* args);
                        


    class Master{
            std::string name;
            const int MAX_NUM_CLIENTS = 7;
            
            bool esp;
            bool connected_to_internet;
            dev_addr_t address;

            //For characteristic writing/reading.
            uint8_t device_conn_id; 
            uint16_t device_gatt_if;
            //The internal client gatt_if to perform characteristic reading/writing
            uint16_t internal_client_gatt_if;
            uint8_t internal_client_conn_id;

            

            //To be implemented. For android compatibility mode.
            uint8_t device_id;


            std::list<uint8_t*> connected_clients;
            std::list<connected_server_params_t> neighbours;





            //Server-side buffer for message transmission
            uint8_t master_tx_buffer[MASTER_TX_BUFFER_SIZE];

            //Server-side buffer for message extra args.
            uint16_t master_message_extra_args[MASTER_EXTRA_ARGS_BUFFER_SIZE];

            //Objects to be used by master class
            Router* router;
            MessageHandler mes_handler;


            

            public:
                
                
               
                Master();
                
                ~Master();
              

                //Start method to initialize all fields of the class.
                void start();

                //Shutdown method to cancel objects on heap.
                void shutdown();
         
                
                std::string get_name();
                void set_name(std::string name);
                
                
                bool is_connected_to_internet();
                void set_connected_to_internet(bool connected_to_internet);

                bool is_esp();
                void set_esp(bool is_esp);

                dev_addr_t get_dev_addr();
                void set_dev_addr(dev_addr_t new_dev_addr);
                
                uint16_t get_device_connection_id();
                void set_device_connection_id(uint16_t device_conn_id);

                uint8_t get_device_gatt_if();
                void set_device_gatt_if(uint16_t gatt_if);

                Router* get_router();
                MessageHandler* get_message_handler();
                uint8_t* get_master_tx_buffer();
                
                uint16_t get_internal_client_gatt_if();
                void set_internal_client_gatt_if(uint16_t internal_gatt_if);

                uint8_t get_internal_client_conn_id();
                void set_internal_client_conn_id(uint8_t conn_id);


                std::list<uint8_t*> get_connected_clients();
                //Add the MAC address ("new address") to the connected client list.
                void add_connected_client(uint8_t* new_address);
                //Remove the MAC address ("address") to the connected client list.
                void remove_connected_client(uint8_t* address);
                

                std::list<connected_server_params_t> get_neighbours();
                //Add the MAC address("new address") to the neighbour list. This MAC address
                //represents a server.
                void add_neighbour(connected_server_params_t new_address);
                //Remove the server represented by "address" by the neighbour list
                void remove_neighbour(connected_server_params_t address);
                


                void update_master_macs(uint8_t* macs,uint16_t gatt_if,uint8_t conn_id,
                                        uint8_t server_id, uint8_t flag);
                void update_master_routing_table (uint8_t* address);

                
                void add_routing_table_entry(dev_addr_t target_addr,
                                    dev_addr_t hop_addr,uint8_t num_hops, uint8_t t_flags);

                void remove_routing_table_entry(dev_addr_t addr);

                //Overloaded version with the routing_params_t structure.
                void add_routing_table_entry(routing_params_t& routing_param);



                dev_addr_t& get_next_hop(dev_addr_t target_addr);
                
                dev_addr_t& get_router_dev_addr();

                std::vector<routing_update_t>get_routing_updates();

                uint16_t* get_master_buffer_extra_args();        

                //Message reception callbacks
                void routing_discovery_request_reception_callback(MessageHeader* header_t, void*args);
                void routing_discovery_response_reception_callback(MessageHeader* header_t, void* args);
                void routing_update_reception_callback(MessageHeader* header_t, void* args);


                //Message transmission callbacks
                void routing_discovery_request_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args);
                void routing_discovery_response_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args);
                void routing_update_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,
                                    void* args);

                //Prepare the routing update message whenever a new client connects.
                ErrStatus send_routing_update(); 

                //Overloaded version of the previous function to propagate the updates.
                ErrStatus send_routing_update(std::array<routing_update_t,ROUTING_UPDATE_ENTRIES_MAX>updates,
                                            uint8_t len);


                //Send the routing response message whenever requested.
                

 
                int16_t read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id);

            
                ErrStatus write_characteristic(uint8_t characteristic, uint8_t* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,uint8_t conn_id);


                ErrStatus send_routing_table(uint8_t* src,uint8_t* dst, uint16_t gatt_if, 
                                        uint8_t conn_id,uint8_t server_id);

                ErrStatus connect(uint8_t * mac_address);
                ErrStatus disconnect(uint8_t* mac_address);



                //This will be implemented with a check if the message is correctly sent.
                ErrStatus send_message(uint16_t gatt_if, uint8_t conn_id,uint8_t* address,
                                    MessageHeader* header_t, uint16_t message_size);

                ErrStatus recv_message(uint16_t gatt_if, uint8_t conn_id, uint8_t* address);

                //Wrapper function for MessageHandler::read and MessageHandler::handle
                void parse_message_receive(uint8_t* packet, uint16_t size);

                //Wrapper function for MessageHandler::send and MessageHandler::handle
                void parse_message_send(MessageHeader* header_t);
                

                

        };

        //Istanza della classe accessibile ovunque nel codice.
        extern Master* master_instance;
}


