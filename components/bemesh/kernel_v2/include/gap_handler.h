/*
 * gap_handler.h
 * Handler for GAP operations
 */

#pragma once
// Block of libraries for ESP functionality
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
//-------------------------------

typedef struct {
  // Advertising:
  esp_ble_adv_data_t adv_data; // Advertising data
  esp_ble_adv_data_t rsp_data; // Advertising Response data
  esp_ble_adv_params_t adv_params; // Advertising parameters
  // Scanning:
  esp_ble_scan_params_t scan_params; // Scanning parameters
  
  uint8_t mode; // 0 for scanning, 1 for advertising.

  // Buffer for storing response customized payload (manufacturer)
  uint8_t *rsp_man_buffer;
  uint8_t rsp_man_buffer_len;
} bemesh_gap_handler;

// Initializes the ble structures in the gap handler h.
int bemesh_gap_handler_init(bemesh_gap_handler* h,
			    uint8_t rsp_buffer,
			    uint8_t rsp_buffer_len);
// Configure the operational mode on the gap handler h.
void bemesh_gap_handler_mode(bemesh_gap_handler* h, uint8_t m);
