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

#include "gatt_def.h"

/*
 * gatts_profile_inst represent an Application Profile object
 * used to establish a GATT connection with one client application
 * (In our case, the bemesh network).
 * The structure is taken from the GATTS example provided by ESP-IDF.
 * The structure contains a single service with a single characteristic,
 * which is exactly the kind of structure we're looking for.
 * PLEASE NOTICE:
 * The structure is slightly different from the example's one, since 
 * BEMESH protocol wont rely on multiple profiles. A single GATTS callback
 * is used, therefore the gatts_cb element was removed.
 */
typedef struct {
  //esp_gatts_cb_t gatts_cb; // Unused since we're using a single profile
  uint16_t gatts_if; // gatts interface (provided by the bt_driver)
  uint16_t app_id; // application id 
  uint16_t conn_id; // connection id
  uint16_t service_handle; 
  esp_gatt_srvc_id_t service_id;
  uint16_t char_handle;
  esp_bt_uuid_t char_uuid;
  esp_gatt_perm_t perm;
  esp_gatt_char_prop_t property;
  uint16_t descr_handle;
  esp_bt_uuid_t descr_uuid;
} gatts_profile_inst;

typedef struct {
  gatts_profile_inst profile_inst;
  esp_attr_value_t char1_val;
  uint8_t char_buffer[GATT_CHAR_BUF_SIZE];
} bemesh_gatts_handler;

bemesh_gatts_handler* bemesh_gatts_handler_init(void);
