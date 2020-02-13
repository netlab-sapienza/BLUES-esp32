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
static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
  [PROFILE_A_APP_ID] = {
    .gatts_cb=gatts_profile_evt_handler,
    .gatts_if=ESP_GATT_IF_NONE,
  }
};

/*
 * GAP
 */

static void _adv_data_setup(esp_ble_adv_data_t* data) {
  data->set_scan_rsp=false; // Set this advertising data as scan response or not
  data->include_name=true;  // Advertising data include device name or not
  data->include_txpower=true;  // Advertising data include TX power
  data->min_interval=0x0006; // 0x06 * 1.25ms = 7.5ms (min connection interval)
  data->max_interval=0x0010; // 0x10 * 1.25ms = 20ms (max connection interval)
  data->appearance=0x00; // External appearance of device
  data->manufacturer_len=0; // Manufacturer data length
  data->p_manufacturer_data=NULL; // Manufacturer data point
  data->service_data_len=0; // Service data length
  data->p_service_data=NULL; // Service data point
  data->service_uuid_len=32; // Service uuid length
  data->p_service_uuid=test_service_uuid128; // Service uuid array point
  // Advertising flag of discovery mode, see BLE_ADV_DATA_FLAG details
  data->flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
  return;
}

static int core_gap_init(bemesh_core_t* core) {
  _adv_data_setup(&core->adv_data);
}

/*
 * GATT
 */

/*
 * GATTS Event Handler. The function handles all the events generated from the Bluedroid
 * stack.
 */
void gatts_profile_evt_handler(esp_gatts_cb_event_t event,
			       esp_gatt_if_t gatts_if,
			       esp_ble_gatts_cb_param_t *params) {
  
  
  
}


int core_init(bemesh_core_t* core) {
  // Register the application profile.
  esp_ble_gatts_app_register(PROFILE_A_APP_ID);
}
