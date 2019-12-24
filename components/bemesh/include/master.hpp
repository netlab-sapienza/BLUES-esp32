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


#include <string>
#define PROFILE_NUM 5






namespace  bemesh{
     


    class Master{

            uint8_t masterId;
            std::string TAG;
            const int MAX_NUM_CLIENTS = 7;
            gatts_profile_inst gl_profile_tab[PROFILE_NUM];
            //NextIdService nextIdService;

            
            
            //Used to be static
            uint8_t adv_config_done = 0;

            public:
                Master(uint8_t id, std::string TAG);
                Master(uint8_t id);
                std::string getTAG();
                uint8_t getId();
                void init();
                /*
                NextIdService getNextIdService(){return nextIdService;}

                void setNextIdService(unsigned int next_id){nextIdService.nextId = next_id;}
                */

                //Read the value of a characteristic stored on a specific service stored in the
                //service table of the esp.

                esp_gatt_rsp_t read_characteristic(struct gatts_profile_inst* table, 
                                        unsigned int table_size,
                                        uint16_t service_handle, uint16_t char_handle,
                                        esp_ble_gatts_cb_param_t *param);

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
                
                
        };
}


