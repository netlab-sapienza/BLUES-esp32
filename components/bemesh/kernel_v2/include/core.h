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

// GAP/GATT handlers.
#include "gap_handler.h"
#include "gatts_handler.h"
#include "gattc_handler.h"
#include "gatt_def.h"

// Core Interface
#include "core_int.h"

/*
 * bda_id_tuple represent the pair <remote_bda,conn_id> where the conn_id 
 * may represent a incoming connection or outgoing connection.
 */
#define CORE_UNUSED_CONN_ID 255
typedef struct {
  esp_bd_addr_t bda;
  uint16_t conn_id;
}bda_id_tuple;

/**
 *  Compares bda1 address with bda2 address.
 *  @param bda1 first address
 *  @param bda2 second address
 *  @return 0 if bda1 equals bda2, 1 otherwise
 */
int bda_equals(esp_bd_addr_t bda1, esp_bd_addr_t bda2);

/*
 * bemesh_core_t represent the main structure for BLE stack interaction.
 * Both GATT(Server and Client) and GAP protocols are handled through the use
 * of gatts_handler, gattc_handler and gap_handler submodules.
 * Users should not directly use any of the previous submodules, as every interaction
 * should pass through the core module first.
 */
typedef struct {
  bemesh_gap_handler *gaph;
  bemesh_gatts_handler *gattsh;
  bemesh_gattc_handler *gattch;

  // Outgoing connections array.
  bda_id_tuple outgoing_conn[GATTC_MAX_CONNECTIONS];
  uint8_t outgoing_conn_len;
  // Incoming connections array.
  bda_id_tuple incoming_conn[GATTS_MAX_CONNECTIONS];
  uint8_t incoming_conn_len;

  // Callback to execute for upper layers.
  kernel_int_cb handler_cb[KERNEL_EVT_NUM];
  bemesh_evt_params_t handler_cb_args;
} bemesh_core_t;


bemesh_core_t* bemesh_core_init(void);
// Returns the current dev bda.
uint8_t *bemesh_core_get_bda(bemesh_core_t* c);
/* GAP HANDLING */
// Scanning ops
// start the scan proc.
int bemesh_core_start_scanning(bemesh_core_t* c, uint16_t timeout);
// stop the scan proc.
int bemesh_core_stop_scanning(bemesh_core_t* c);
// returns the status of the scanning proc.
uint8_t bemesh_core_is_scanning(bemesh_core_t* c);
// returns the scan complete status flag of gaph
uint8_t bemesh_core_scan_complete(bemesh_core_t* c);
/* returns the scan result array length.
 * bemesh_core_scan_complete function should be called first
 * to know if scan procedure is complete.
 */
uint8_t bemesh_core_get_scan_result_len(bemesh_core_t* c);
/* Returns a pointer to an array containing the scan results.
 * Refer to bemesh_dev_t definition to 
 */
bemesh_dev_t *bemesh_core_get_scan_result(bemesh_core_t* c);
// Advertising ops
int bemesh_core_start_advertising(bemesh_core_t* c);
int bemesh_core_stop_advertising(bemesh_core_t* c);
uint8_t bemesh_core_is_advertising(bemesh_core_t* c);

/* GATT HANDLING */
/* establish a connection with a remote dev that has bda bda
 */
int bemesh_core_connect(bemesh_core_t* c, esp_bd_addr_t bda);
/* disconnects from a remote dev that has bda bda */
int bemesh_core_disconnect(bemesh_core_t* c, esp_bd_addr_t bda);
int bemesh_core_write(bemesh_core_t* c, uint16_t conn_id, uint8_t *src, uint16_t len, uint8_t is_notify);
int bemesh_core_read(bemesh_core_t* c, uint16_t conn_id, uint8_t *dest, uint16_t len);

// Install the handler for kernel events
void bemesh_core_install_callback(bemesh_core_t *c, bemesh_kernel_evt_t evt, kernel_int_cb cb);
// Uninstall the handler for kernel events
void bemesh_core_uninstall_callback(bemesh_core_t *c, bemesh_kernel_evt_t evt);

