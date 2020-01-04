
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


extern "C"{
    #include "kernel.h"
}

namespace bemesh{
    class Callback{
        
        static void init_callback(uint8_t type);
        static void notify_callback(void);
        static void server_update_callback(uint8_t* macs);

        public:
            void operator()(void);
            Callback();
    };
}




