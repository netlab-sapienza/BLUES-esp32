/*
 * core.c
 * Handles operations directly with the ESP module.
 */

#include "core.h"

void gatts_profile_evt_handler(esp_gatts_cb_event_t event,
			       esp_gatt_if_t gatts_if,
			       esp_ble_gatts_cb_param_t *params);

// GATT Profile, in our case, only one profile will be builtw
#define PROFILE_A_APP_ID 0

/*
 * GATTS Event Handler. The function handles all the events generated from the Bluedroid
 * stack.
 */

void gatts_profile_evt_handler(esp_gatts_cb_event_t event,
			       esp_gatt_if_t gatts_if,
			       esp_ble_gatts_cb_param_t *params) {
  //TODO  
}

/*
 * core1 Represent the static bemesh_core_t element present in the 
 * firmware.
 * No other bemesh_core_t elements may be instantiated during the execution
 * of the firmware.
 */
bemesh_core_t core1;
// Get the core1 pointer.
static bemesh_core_t* get_core1(void) {
  return &core1;
}


bemesh_core_t* bemesh_core_init(void) {
  bemesh_core_t* core_ptr=get_core1();
  // Register the application profile.
  esp_ble_gatts_app_register(PROFILE_A_APP_ID);

  // Initialize the core
  //TODO

  return core_ptr;
}
