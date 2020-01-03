
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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






#include "services.hpp"
#include "constant.hpp"


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
     


    class Master{
            std::string name;
            const int MAX_NUM_CLIENTS = 7;
            
            bool esp;
            bool connected_to_internet;
            uint8_t* address;
            uint8_t device_conn_id; //for Android compatibility mode.
            uint16_t device_gatt_if;
            //Used to be static
            uint8_t adv_config_done = 0;

            //Server-side buffer for message transmission
            uint8_t master_tx_buffer[MASTER_TX_BUFFER_SIZE];


            //Objects to be used by master class
            Router* router;
            MessageHandler mes_handler;


            //Private function to convert a uint8_t* to a dev_addr_t.
            dev_addr_t _build_dev_addr(uint8_t* address);


            public:
                
                
                //To be deleted
                Master(uint8_t id, std::string TAG);
                Master(uint8_t id);
                Master();
                

                //Start method to initialize all fields of the class.
                void start();

                //Shutdown method to cancel objects on heap.
                void shutdown();
                

                


                ~Master();
                Master(bool is_esp, bool connected_to_internet);


                
                
                std::string get_name();
                void set_name(std::string name);
                
                
                bool is_connected_to_internet();
                void set_connected_to_internet(bool connected_to_internet);

                bool is_esp();
                void set_esp(bool is_esp);

                uint8_t* get_dev_addr();
                void set_dev_addr(uint8_t* new_dev_addr);
                
                uint16_t get_device_connection_id();
                void set_device_connection_id(uint16_t device_conn_id);

                uint8_t get_device_gatt_if();
                void set_device_gatt_if(uint16_t gatt_if);

                Router* get_router();
                MessageHandler* get_message_handler();
                uint8_t* get_master_tx_buffer();
 
                int16_t read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id);

            
                ErrStatus write_characteristic(uint8_t characteristic, dev_addr_t address, void* buffer,
                                        uint8_t buffer_size, uint16_t gattc_if,uint16_t conn_id);
            


                                        

                
        };

        //Istanza della classe accessibile ovunque nel codice.
        extern Master* master_istance;
}


