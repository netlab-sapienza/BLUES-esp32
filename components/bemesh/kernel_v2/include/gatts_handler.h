/*
 * gatts_handler.h
 * Handler for GATT-Server related operations
 */

#pragma once
// Block of libraries for ESP functionality
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

typedef struct {

} bemesh_gatts_handler;

bemesh_gatts_handler* bemesh_gatts_handler_init(void);
