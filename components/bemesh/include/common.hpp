
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
#include <memory>
#include <iostream>
#include <algorithm>


extern "C"{
    #include "kernel.h"
    #include "gatts_table.h"
}


    struct task_param_write_t{
        uint8_t conn_id;
        uint16_t gatt_if;
        uint8_t characteristic;
        uint8_t* buffer;
        uint16_t buffer_size;
        task_param_write_t();
    };


bool contains_mac(std::list<uint8_t*> ls,uint8_t* address, uint8_t address_size);
bemesh::dev_addr_t _build_dev_addr(uint8_t* address);
void _print_mac_address(uint8_t* address);
void write_characteristic_task(void* task_param);
