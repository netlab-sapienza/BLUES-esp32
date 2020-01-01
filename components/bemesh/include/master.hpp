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
#include "rtable.hpp"
#include "bemesh_messages.hpp"
#include "bemesh_status.hpp"
#include "gatts_table.h"


extern "C"{
    #include "kernel.h"
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

            public:
                
                
                //to be deleted
                Master(uint8_t id, std::string TAG);
                Master(uint8_t id);
                Master();

                


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



                void init();
                /*
                NextIdService getNextIdService(){return nextIdService;}

                void setNextIdService(unsigned int next_id){nextIdService.nextId = next_id;}
                */

                //Read the value of a characteristic stored on a specific service stored in the
                //service table of the esp.

                int write_characteristic(struct gatts_profile_inst* table,
                                        unsigned int table_size, uint16_t service_handle, 
                                        uint16_t char_handle, void* value);
                        
                //void addService()
                //void addCharacteristic
                void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
                void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
                
                
                //Callback for A profile and its services.
                void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
                
                
                
                void ble_indicate(uint8_t value, uint16_t id);
                
                int16_t read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id);

            
                ErrStatus write_characteristic(uint8_t characteristic, dev_addr_t address, void* buffer,
                                        uint8_t buffer_size, uint16_t gattc_if,uint16_t conn_id);
            


                                        

                
        };
}


