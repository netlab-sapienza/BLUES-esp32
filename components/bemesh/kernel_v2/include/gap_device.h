/*
 * gap_device.h
 */

#pragma once
#include "esp_bt_defs.h" // ESP_BD_ADDR_LEN
#include <stdint.h>

typedef struct {
  esp_bd_addr_t bda; // Bluetooth Device Address
  int rssi; // received RSSI
} bemesh_dev_t;
