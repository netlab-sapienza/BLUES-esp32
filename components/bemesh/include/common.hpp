
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

namespace bemesh{
    enum connection_policy_t{
        Minimum_rssi_value_policy = 0,
        Maximum_rssi_value_policy = 1,
        Random_policy = 2,
        Fcfs_policy = 3,
    };


    //Other fields to be added
    struct ping_data_t{
        dev_addr_t sender;
        uint8_t pong_flag;
        ping_data_t();
        ping_data_t(dev_addr_t s, uint8_t pflag);
        bool operator ==(const ping_data_t & cc) ;
        bool operator !=(const ping_data_t & cc) ;
    };


    struct task_param_write_t{
        uint8_t conn_id;
        uint16_t gatt_if;
        uint8_t characteristic;
        uint8_t* buffer;
        uint16_t buffer_size;
        task_param_write_t();
    };

    struct connected_server_params_t{
        uint8_t server_id;  //could be one of SERVER_S1, SERVER_S2, SERVER_S3
        uint16_t gatt_if;
        uint8_t conn_id;
        dev_addr_t server_mac_address;
        connected_server_params_t();
        connected_server_params_t(uint8_t server_id,uint16_t gatt_if,uint8_t conn_id,
                                dev_addr_t server_mac);

        bool operator ==  (const connected_server_params_t& cc);
        bool operator != (const connected_server_params_t& cc);

    };

    bool same_addresses(dev_addr_t addr1,dev_addr_t addr2,uint8_t address_size);
    bool contains_mac(std::list<uint8_t*> ls,uint8_t* address, uint8_t address_size);
    bool contains_server(std::list<connected_server_params_t> ls,uint8_t server_id);
    bemesh::dev_addr_t _build_dev_addr(uint8_t* address);
    void _print_mac_address(uint8_t* address);
    void write_characteristic_task(void* task_param);
}