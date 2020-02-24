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

// Core Interface
#include "core_int.h"


typedef struct gattc_profile_inst {
  // No gattc_cb. All profiles will share the same handler.
  // esp_gattc_cb_t gattc_cb;
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
  esp_bt_uuid_t remote_filter_char_uuid;

  // Flag to indicate if server is valid
  // a server is valid iff it contains a specific service.
  uint8_t server_valid_flag;

  // Callback to pass events to the core lib.
  kernel_cb core_cb;
  // It will rely on params struct built inside the core.
  bemesh_evt_params_t *core_cb_args;
} bemesh_gattc_handler;

bemesh_gattc_handler *bemesh_gattc_handler_init(void);
// Open connection with a remote device.
// Returns -1 if no free gatt intefaces are available
uint8_t bemesh_gattc_open(bemesh_gattc_handler* h,
                          esp_bd_addr_t remote_bda,
                          esp_ble_addr_type_t remote_addr_type);

void bemesh_gattc_handler_install_cb(bemesh_gattc_handler *h,
                                     kernel_cb cb, bemesh_evt_params_t *params);

void bemesh_gattc_handler_uninstall_cb(bemesh_gattc_handler *h);

void bemesh_gattc_handler_write(bemesh_gattc_handler *h, uint16_t conn_id,
                                uint8_t *data, uint16_t data_len, uint8_t resp);
