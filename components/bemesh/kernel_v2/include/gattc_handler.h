/*
 * gattc_handler.h
 * Handler for GATT-Client related operations
 */

#pragma once
// Block of libraries for ESP functionality
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#include "gatt_def.h"

typedef struct gattc_profile_inst {
  //esp_gattc_cb_t gattc_cb; // No gattc_cb. All profiles will share the same handler.
  uint16_t gattc_if; 
  uint16_t app_id;
  uint16_t conn_id;
  uint16_t service_start_handle;
  uint16_t service_end_handle;
  uint16_t char_handle;
  esp_bd_addr_t remote_bda;
} gattc_profile_inst;

typedef struct bemesh_gattc_handler {
  gattc_profile_inst profile_inst_vect[GATTC_APP_PROFILE_INST_LEN];
  esp_bt_uuid_t remote_filter_service_uuid;

  // Flag to indicate if server is valid
  // a server is valid iff it contains a specific service.
  uint8_t server_valid_flag;
} bemesh_gattc_handler;

bemesh_gattc_handler *bemesh_gattc_handler_init(void);
// Open connection with a remote device. Returns -1 if no free gatt intefaces are available
uint8_t bemesh_gattc_open(bemesh_gattc_handler* h, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type);
