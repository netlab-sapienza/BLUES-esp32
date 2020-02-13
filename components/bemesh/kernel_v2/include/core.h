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

#define PROFILE_NUM 1
#define GATT_NUM_CALLBACKS 25

typedef void (*gatt_cb_ptr)(void*);

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

int core_init(bemesh_core_t* core);


