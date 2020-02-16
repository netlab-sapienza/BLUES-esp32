/*
 * core.c
 * Handles operations directly with the ESP module.
 */

#include "core.h"
#include "esp_log.h"
#include "nvs_flash.h"

// logging tag
static const char* TAG = "core";

// Since we want one and only one bemesh_core_t we will statically define it.
static bemesh_core_t core1;
static bemesh_core_t *get_core1_ptr(void) {
  return &core1;
}

static void gatts_ext_handler_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler *h, void* args);

void core_peripheral_init(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
  esp_bt_controller_config_t bt_cfg=BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
  esp_bluedroid_init();
  esp_bluedroid_enable();
  return;
}

bemesh_core_t* bemesh_core_init(void) {
  core_peripheral_init();
  bemesh_core_t *core=get_core1_ptr();
  core->gaph=bemesh_gap_handler_init(NULL, 0, NULL, 0); // TODO: Add rsp and srv_uuid buffers
  // Link the gatts_handler to core1
  core->gattsh=bemesh_gatts_handler_init();
  bemesh_gatts_handler_install_cb(core->gattsh, gatts_ext_handler_cb, (void*)core);
  
  //TODO
  return NULL;
}


static void gatts_ext_handler_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, struct bemesh_gatts_handler *h, void* args) {
  bemesh_core_t* core=(bemesh_core_t*)args;
  switch(event) {
  case ESP_GATTS_CONNECT_EVT:
    // If a user connects from us, start advertising back again
    //bemesh_gap_handler_mode(core->gaph, GAP_HANDLER_MODE_PERIPHERAL);
    break;
  case ESP_GATTS_DISCONNECT_EVT:
    // If a user disconnects from us, start advertising back again
    bemesh_gap_handler_mode(core->gaph, GAP_HANDLER_MODE_PERIPHERAL);
    break;
  default:
    break;
  }
  return;
}
