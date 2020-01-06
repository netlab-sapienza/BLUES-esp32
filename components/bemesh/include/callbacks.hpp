
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <stdint.h>


#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"

#include "esp_gatts_api.h"
#include "esp_bt_defs.h"


#include "freertos/event_groups.h"
#include "esp_system.h"


#define FUNCTOR_TAG "FUNCTOR"



#include "master.hpp"
#include "slave.hpp"
#include "routing.hpp"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

extern "C"{
    #include "kernel.h"
}

namespace bemesh{
    class Callback{
        
        static void init_callback(uint8_t type);
        static void notify_callback(uint16_t gattc_if,uint8_t conn_id,uint8_t charact);
        static void server_update_callback(uint8_t* macs,uint8_t flags);
        static void exchange_routing_table_callback(uint8_t* src,uint8_t* dest,
                                                uint16_t gatt_if,uint8_t conn_id);
        static void send_routing_table_callback(uint8_t* src, uint8_t* dest, uint16_t gatt_if,
                                                uint8_t conn_id);

        static void received_packet_callback(uint8_t* packet,uint16_t size);                                            

        public:
            void operator()(void);
            Callback();
    };
}




