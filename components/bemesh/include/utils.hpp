

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

#include <stdlib.h>
#include "freertos/event_groups.h"
#include "esp_system.h"


/*
 *  	MACROS
 */
namespace bemesh{
    class Utils{
        public:
            static bool Isconnecting    = false;
            static bool stop_scan_done  = false;

            static void start_scan(void);
    };
}


