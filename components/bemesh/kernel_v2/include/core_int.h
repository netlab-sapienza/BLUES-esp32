/*
 * core_int.h
 */

#pragma once
#include <stdint.h>
#include "gap_device.h"

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
  // On incoming connection params
  struct inc_conn_param {
    esp_bd_addr_t* remote_bda;
  } conn;
} bemesh_evt_params_t;
/*
Example of use for this kind of param
int foo(bemesh_evt_params_t param) {
  bemesh_dev_t* device_list = param->scan.result;
  uint16_t len = param->scan.len;
}
*/

/*
 * Scan the environment
 * TODO
 */
void scan_environment(uint8_t timeout);
