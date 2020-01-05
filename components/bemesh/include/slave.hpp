

#pragma once
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


#include <stdlib.h>

#include "rtable.hpp" //For dev_addr_t data type.
#include <stdint.h>
#include <string>


#include "bemesh_status.hpp"
#include "message_handler_v2.hpp"
#include "routing.hpp"
#include "constant.hpp"
#include "common.hpp"
#include <stdlib.h>
#include <iostream>


extern "C"{
    #include "kernel.h"
    #include "gatts_table.h"
}

namespace bemesh{
    static std::string comm_char = "Hello everyone";

    class Slave{
        uint8_t* address;
        uint16_t server_conn_id;
        uint16_t device_conn_id; // also for android compatibility mode.
        uint8_t device_gatt_if;
        bool esp;
        bool connected_to_internet;
        std::string name;


        //Objects used by the slave istance.
        Router* router;
        MessageHandler mes_handler;

        //Buffer for message send/receive
        uint8_t slave_tx_buffer[SLAVE_TX_BUFFER_SIZE];
        
     

        public:
            Slave();
            ~Slave();
            Slave(bool is_esp, bool connected_to_internet);

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

            

            void add_routing_table_entry(dev_addr_t target_addr,
                                dev_addr_t hop_addr,uint8_t num_hops, uint8_t t_flags);

            void remove_routing_table_entry(dev_addr_t addr);
            dev_addr_t& get_next_hop(dev_addr_t target_addr);
                


            int16_t read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id);

            
            ErrStatus write_characteristic(uint8_t characteristic, dev_addr_t address, uint8_t* buffer,
                                        uint8_t buffer_size, uint16_t gattc_if,uint16_t conn_id);
            

            void sayHello(){
                std::cout<<"Hello"<<std::endl;
            }

            //This primitive directly interact with the communication characteristic (IDX_CHAR_A)
            ErrStatus send_message(uint16_t gattc_if, uint16_t conn_id, char* message,
                                    uint8_t message_size);


            //Util to print slave status and to verify correct data initialization.
            void print_status();


            

    };

    extern Slave* slave_instance;
}