/*
 * core_int.h
 */

#pragma once
#include <stdint.h>
#include "gap_device.h"

#define KERNEL_EVT_NUM 3
typedef enum {
  ON_SCAN_END=0, // Scan end event
  ON_MSG_RECV=1, // Message recv event
  ON_INC_CONN=2, // Incoming connection event
  ON_OUT_CONN=3, // Outgoing connection event
  ON_DISCONN=4, // Disconnection event
  ON_READ_REQ=5, // Read request event
} bemesh_kernel_evt_t;

typedef union {
  // Scan result params
  struct scan_result_param {
    bemesh_dev_t* result;
    uint16_t len;
  } scan;
  // On message receive params
  struct message_recv_param {
    esp_bd_addr_t* remote_bda;
    uint8_t* payload;
    uint16_t len;
  } recv;
  // On incoming/outgoing/ connection/disconnection params
  struct conn_param {
    esp_bd_addr_t* remote_bda;
    uint8_t conn_id;    
  } conn;
} bemesh_evt_params_t;
/*
Example of use for this kind of param
int foo(bemesh_evt_params_t param) {
  bemesh_dev_t* device_list = param->scan.result;
  uint16_t len = param->scan.len;
}
*/

// Kernel callback definition
typedef void (*kernel_cb)(bemesh_kernel_evt_t evt, bemesh_evt_params_t* param);

/*
 * Install the cb callback for the Event event.
 * Parameters will be updated before launching the cb
 * Refer to bemesh_kernel_evt_t enum to check how events are defined.
 * Refer to bemesh_evt_params_t union to check how params are passed.
 */
void kernel_install_cb(bemesh_kernel_evt_t event, kernel_cb cb);

/*
 * Initializes the underlying kernel.
 */
int kernel_init(void);

/*
 * Transfer the src buffer of len bytes to another device with bda address.
 */
void send_payload(esp_bd_addr_t bda, uint8_t *src, uint16_t len);

/*
 * Tries to connect to a device with bda address.
 * returns 0 if no error occurred
 */
uint8_t connect_to(esp_bd_addr_t bda);

/*
 * Scan the environment
 * TODO
 */
void scan_environment(uint8_t timeout);

/*
 * Get the device bda.
 */
uint8_t *get_own_bda(void);
