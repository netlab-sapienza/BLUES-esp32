/*
 * gatts_callback.c
 * Container for GATTS callbacks.
 */

#pragma once
#include "core.h"

/*
 * Event descriptions
 * ESP_GATTS_REG_EVT : When register application id
 * ESP_GATTS_READ_EVT : When gatt client request read operation
 * ESP_GATTS_WRITE_EVT : When gatt client request write operation
 * ESP_GATTS_EXEC_WRITE_EVT : When gatt client request execute write
 * ESP_GATTS_MTU_EVT : When set mtu complete
 * ESP_GATTS_CONF_EVT : When receive confirm
 * ESP_GATTS_UNREG_EVT : When unregister application id
 * ESP_GATTS_CREATE_EVT : When create service complete
 * ESP_GATTS_ADD_INCL_SRVC_EVT : When add included service complete
 * ESP_GATTS_ADD_CHAR_EVT : When add characteristic complete
 */ 

// ESP_GATTS_REG_EVT : When register application id
void gatts_reg_evt_cb(esp_gatt_if_t gatts_if,
		      esp_ble_gatts_cb_param_t *params,
		      void* args) {
  bemesh_core_t* core=(bemesh_core_t*)args;
  // Setting up the profile table
  core->gatts_profile_tab[PROFILE_IDX].service_id.is_primary=true;
  core->gatts_profile_tab[PROFILE_IDX].service_id.id.inst_id=0x00;
  core->gatts_profile_tab[PROFILE_IDX].service_id.id.uuid.len=ESP_UUID_LEN_16;
  //  core->gatts_profile_tab[PROFILE_IDX].service_id.id.uuid.uuid.uuid16;
}
