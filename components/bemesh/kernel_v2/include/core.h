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

// Number of avaiable profiles
#define PROFILE_NUM 1
#define PROFILE_IDX 0
#define GATT_NUM_CALLBACKS 25

typedef void (*gatt_cb_ptr)(void*);

typedef struct {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
} gatts_profile_inst;

/*
 * bemesh_core_t represent the main core structure containing relevant elements
 * for GATT and GAP functionalities.
 * A node may contain a single bemesh_core_t structure which will handle direct
 * operations with the ESP32 module.
 * Remember that every interaction with ESP may be done ONLY through this structure
 * in order to mantain abstraction valid.
 */
typedef struct {
  // Application profile array
  gatts_profile_inst gatts_profile_tab[PROFILE_NUM];
  // Advertising / Response data
  esp_ble_adv_data_t adv_data;

  // Callbacks relative to GATT profile.
  // Each callback is enumerated
  gatt_cb_ptr gatt_cb_vect[GATT_NUM_CALLBACKS];
  // Each callback has its arguments
  void* gatt_cb_args[GATT_NUM_CALLBACKS];
  
} bemesh_core_t;

bemesh_core_t* bemesh_core_init(void);


