/*
 * core.h
 * Handles operations directly with the ESP module.
 */

#pragma once
// ESP Libraries
#include "esp_bt.h" // BT controller and VHCI configuration procedures.
#include "esp_bt_main.h" // Bluedroid Stack.
#include "esp_gap_ble_api.h" // GAP configuration, such advertising and connection params
#include "esp_gatts_api.h" // GATT configuration, services and characteristics
#include "esp_gatt_defs.h" // GATT definitions.

#include "gap_handler.h"


typedef struct {
  bemesh_gap_handler* gaph;
} bemesh_core_t;

bemesh_core_t* bemesh_core_init(void);
