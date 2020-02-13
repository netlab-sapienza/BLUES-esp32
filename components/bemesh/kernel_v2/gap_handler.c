/*
 * gap_handler.c
 * Handler for GAP operations
 */

#include "gap_handler.h"

// Configure the advertising data for the module.
static void setup_advertising_data(esp_ble_adv_data_t* adv) {
  adv->set_scan_rsp=false; // This is not the response structure.
  adv->include_name=false; // Include the name in the adv packet.
  adv->min_interval=0x06; // Minimum advertising interval : 0x06 * 1.25ms = 7.5 ms.
  adv->max_interval=0x10; // Maximum advertising interval : 0x10 * 1.25ms = 20 ms.

  return;
}

// Configure the response data for the module.
static void setup_resp_data(esp_ble_adv_data_t* adv) {
  return;
}

// Configure the advertising paramters for the module.
static void setup_advetising_params(esp_ble_adv_params_t* params) {
  return;
}

// Configure the advertising paramters for the module.
static void setup_scanning_params(esp_ble_scan_params_t* params) {
  return;
}

// Initializes the ble structures in the gap handler h.
int bemesh_gap_handler_init(bemesh_gap_handler* h) {
  // Setup the structures of gap handler
  setup_advertising_data(&h->adv_data); // adv data
  setup_resp_data(&h->rsp_data); // rsp data
  setup_advetising_params(&h->adv_params); // adv params
  setup_scanning_params(&h->scan_params); // scan params
  
  return 0;
}

// Configure the operational mode on the gap handler h.
void bemesh_gap_handler_mode(bemesh_gap_handler* h, uint8_t m) {
  return;
}

