
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



#include <list>
#include <stdint.h>
#include "constant.hpp"
#include "rtable.hpp"



bool contains_mac(std::list<uint8_t*> ls,uint8_t* address, uint8_t address_size);
bemesh::dev_addr_t _build_dev_addr(uint8_t* address);
void _print_mac_address(uint8_t* address);