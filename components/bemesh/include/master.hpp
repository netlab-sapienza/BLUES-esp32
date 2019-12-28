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



#include <string>





#define NUM 2

namespace  bemesh{
     


    class Master{
            std::string name;
            const int MAX_NUM_CLIENTS = 7;
            
            bool esp;
            bool connected_to_internet;
            dev_addr_t address;
            

            uint8_t master_id; //for Android compatibility mode.



            
            
            //Used to be static
            uint8_t adv_config_done = 0;

            public:
                
                
                //to be deleted
                Master(uint8_t id, std::string TAG);
                Master(uint8_t id);


                ~Master();
                Master(bool is_esp, bool connected_to_internet);


                
                
                std::string get_name();
                void set_name(std::string name);
                
                uint8_t get_id();
                void set_id(uint8_t new_id);


                bool is_connected_to_internet();
                void set_connected_to_internet(bool connected_to_internet);

                bool is_esp();
                void set_esp(bool is_esp);

                dev_addr_t get_dev_addr();
                void set_dev_addr(dev_addr_t new_dev_addr);





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
                
                uint16_t read_characterstic(uint16_t characteristic, dev_addr_t dev_addr);
                                        

                
        };
}


