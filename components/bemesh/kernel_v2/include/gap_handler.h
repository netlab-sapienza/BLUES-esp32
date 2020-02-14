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
//-------------------------------
#include "gap_def.h" // GAP parameters definition
#include "gap_device.h" // bemesh_dev_t struct

typedef struct {
  // Advertising:
  esp_ble_adv_data_t adv_data; // Advertising data
  esp_ble_adv_data_t rsp_data; // Advertising Response data
  esp_ble_adv_params_t adv_params; // Advertising parameters
  uint8_t adv_data_set; // flag for adv_data setup
  uint8_t rsp_data_set; // flag for rsp_data setup
  // Scanning:
  esp_ble_scan_params_t scan_params; // Scanning parameters
  uint8_t scan_params_complete;
  // Array of found devices
  bemesh_dev_t found_devs_vect[GAP_HANDLER_SCAN_DEVS_MAX];
  uint8_t found_devs;
  
  uint8_t mode; // 0 for scanning, 1 for advertising.

  // Buffer for storing response customized payload (manufacturer)
  uint8_t *rsp_man_buffer;
  uint8_t rsp_man_buffer_len;
} bemesh_gap_handler;

// Initializes the ble structures in the gap handler h.
bemesh_gap_handler* bemesh_gap_handler_init(uint8_t *rsp_buffer,
					    uint8_t rsp_buffer_len,
					    uint8_t *srv_uuid_buffer,
					    uint8_t srv_uuid_len);
// Configure the operational mode on the gap handler h.
void bemesh_gap_handler_mode(bemesh_gap_handler* h, uint8_t m);
