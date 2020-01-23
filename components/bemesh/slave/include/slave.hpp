

#pragma once
#include <stdlib.h>

#include "rtable.hpp" //For dev_addr_t data type.
#include "bemesh_status.hpp"
#include "message_handler_v2.hpp"
#include "routing.hpp"
#include "constant.hpp"
#include "common.hpp"
#include "bemesh_error.hpp"

#include <stdlib.h>
#include <iostream>
#include <list>
#include <assert.h>
#include <stdint.h>
#include <string>


extern "C"{
#include "esp_log.h"
#include "kernel.h"
#include "gatts_table.h"
}

namespace bemesh{
    static std::string comm_char = "Hello everyone";

    void slave_reception_callback(MessageHeader* header_t,void* args);
    void slave_transmission_callback(uint8_t* buffer,uint8_t size,MessageHeader* header_t,void*args);


    class Slave{
        uint8_t* address;
        uint8_t* server_dev_addr;
        uint16_t server_conn_id;
        uint16_t device_conn_id; // also for android compatibility mode.
        uint8_t device_gatt_if;
        bool esp;
        bool connected_to_internet;
        std::string name;
        


        dev_addr_t src_address_to_send;



        //Delete it if it is not necessary
        std::list<ping_data_t> ping_response_list;

        //Objects used by the slave istance.
        Router* router;
        MessageHandler mes_handler;

        //Buffer for message send/receive
        uint8_t slave_tx_buffer[SLAVE_TX_BUFFER_SIZE];

        //256 bytes for message extra arguments.
        uint8_t slave_message_extra_args[SLAVE_EXTRA_ARGS_BUFFER_SIZE];

        
     

        public:
            Slave();
            ~Slave();
            
            void start();
            void shutdown();

            std::string get_name();
            void set_name(std::string name);

            bool is_connected_to_internet();
            void set_connected_to_internet(bool connected_to_internet);

            bool is_esp();
            void set_esp(bool is_esp);

            uint8_t* get_dev_addr();
            void set_dev_addr(uint8_t* new_dev_addr);

            uint16_t get_server_connection_id();
            void set_server_connection_id(uint16_t conn_id);

            uint16_t get_device_connection_id();
            void set_device_connection_id(uint16_t device_conn_id);

            uint8_t get_device_gatt_if();
            void set_device_gatt_if(uint16_t gatt_if);

            Router* get_router();
            MessageHandler* get_message_handler();
            uint8_t * get_slave_tx_buffer();

            uint8_t* get_server_dev_addr();
            void set_server_dev_addr(uint8_t* addr);
            
            

            void add_routing_table_entry(dev_addr_t target_addr,
                                dev_addr_t hop_addr,uint8_t num_hops, uint8_t t_flags);

            void remove_routing_table_entry(dev_addr_t addr);
            dev_addr_t& get_next_hop(dev_addr_t target_addr);

            uint8_t* get_slave_message_extra_args();



            //The client will be interested only in routing update packets.
            void routing_update_reception_callback(MessageHeader* header_t, void* args);
            void ping_reception_callback(MessageHeader* header_t, void* args);
            
            void routing_update_transmission_callback(uint8_t* message, uint8_t size,
                                                MessageHeader* header_t,void* args);

            void ping_transmission_callback(uint8_t* message, uint8_t size, 
                                                MessageHeader* header_t,void * args);

            void add_ping_response(ping_data_t rsp);
            void remove_ping_response(ping_data_t rsp);
            std::list<ping_data_t> get_ping_response();
            

            //To be implemented once we have the enhanced versions of the scanning functions.
            ErrStatus connect(uint8_t* address);
            ErrStatus disconnect(uint8_t* address);            

            uint8_t* read_characteristic(uint8_t characteristic,uint16_t gattc_if, uint16_t conn_id);


            
            ErrStatus write_characteristic(uint8_t characteristic, uint8_t* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint8_t conn_id,write_policy_t policy);
            

            void sayHello(){
                std::cout<<"Hello"<<std::endl;
            }

            //The characteristic argument is used to send the message on the correct characteristic interface.
            ErrStatus send_message(uint16_t gattc_if, uint8_t conn_id, uint8_t* address,MessageHeader* message, uint8_t characteristic);

            ErrStatus recv_message(uint16_t gatt_fi, uint8_t conn_id, uint8_t* address);




            //Util to print slave status and to verify correct data initialization.
            void print_status();

            //ping_server is defined to print 
            ErrStatus ping_server(uint16_t gatt_if, uint8_t conn_id, uint8_t* mac_address,
                                uint8_t pong_flag);


            

    };

    extern Slave* slave_instance;
}
