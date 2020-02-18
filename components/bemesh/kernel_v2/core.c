/*
 * core.c
 * Handles operations directly with the ESP module.
 */

#include "core.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt_device.h" // esp_bt_dev_get_address

// logging tag
static const char* TAG = "core";

// Since we want one and only one bemesh_core_t we will statically define it.
static bemesh_core_t core1;
static bemesh_core_t *get_core1_ptr(void) {
  return &core1;
}


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

static void log_own_bda(void) {
  char buf[64];
  int wb=sprintf(buf, "Initializing device's core with BDA: ");
  uint8_t* bda=bemesh_core_get_bda(get_core1_ptr());
  for(int i=0;i<ESP_BD_ADDR_LEN; ++i) {
    wb+=sprintf(buf+wb, "%02X.", bda[i]);
  }
  ESP_LOGI(TAG, "%s", buf);
  return;
}

bemesh_core_t* bemesh_core_init(void) {
  core_peripheral_init();
  bemesh_core_t *core=get_core1_ptr();
  // Link the gatts_handler to core1 and initialize it
  core->gattsh=bemesh_gatts_handler_init();
  // Link the gattc_handler to core1 and initialize it
  core->gattch=bemesh_gattc_handler_init();
  
  // Link the gap_handler to core1 and initialize it
  core->gaph=bemesh_gap_handler_init(NULL, 0, NULL, 0); // TODO: Add rsp and srv_uuid buffers
  // Print device BDA.
  log_own_bda();
  //TODO
  return core;
}

// Returns the current dev bda. TODO: increase descr.
uint8_t *bemesh_core_get_bda(bemesh_core_t* c) {
  return esp_bt_dev_get_address();
}

/* GAP HANDLING */
// Scanning ops
int bemesh_core_start_scanning(bemesh_core_t* c, uint16_t timeout) { // start the scan proc.
  return bemesh_gap_handler_start_scanning(c->gaph, timeout);
}

int bemesh_core_stop_scanning(bemesh_core_t* c) { // stop the scan proc.
  bemesh_gap_handler_stop_scanning(c->gaph);
  return 0;
}
uint8_t bemesh_core_scan_complete(bemesh_core_t* c) { // returns the scan complete status flag of gaph
  return bemesh_gap_handler_scan_complete(c->gaph);
}
uint8_t bemesh_core_is_scanning(bemesh_core_t* c) { // returns the status of the scanning proc.
  return (c->gaph->flags&O_SCN);
}
/* returns the scan result array length.
 * bemesh_core_scan_complete function should be called first
 * to know if scan procedure is complete.
 */
uint8_t bemesh_core_get_scan_result_len(bemesh_core_t* c) {
  return bemesh_gap_handler_get_scan_res_len(c->gaph);
}
/* Returns a pointer to an array containing the scan results.
 * Refer to bemesh_dev_t definition to 
 */
bemesh_dev_t *bemesh_core_get_scan_result(bemesh_core_t* c) {
  return bemesh_gap_handler_get_scan_res(c->gaph);
}
// Advertising ops
int bemesh_core_start_advertising(bemesh_core_t* c) {
  return bemesh_gap_handler_start_advertising(c->gaph);
}
// TODO: Add descr
int bemesh_core_stop_advertising(bemesh_core_t* c) {
  bemesh_gap_handler_stop_advertising(c->gaph);
  return 0;
}
// TODO: Add descr
uint8_t bemesh_core_is_advertising(bemesh_core_t* c) {
  return (c->gaph->flags&O_ADV);
}

/* GATT HANDLING */
/* establish a connection with a remote dev that has bda bda
 */
int bemesh_core_connect(bemesh_core_t* c, esp_bd_addr_t bda) {
  // Obfuscate gatts connection event handler
  ESP_LOGW(TAG, "Obfuscating gatts");
  c->gattsh->flags|=O_IGNCONN;
  int ret=bemesh_gattc_open(c->gattch, bda, 0);
  return ret;
}
/* disconnects from a remote dev that has bda bda
 * TODO: Add descr
 */
int bemesh_core_disconnect(bemesh_core_t* c, esp_bd_addr_t bda);
// TODO: Add descr
int bemesh_core_write(bemesh_core_t* c, esp_bd_addr_t bda, uint8_t *src, uint16_t len);
// TODO: Add descr
int bemesh_core_read(bemesh_core_t* c, esp_bd_addr_t bda, uint8_t *dest, uint16_t len);

