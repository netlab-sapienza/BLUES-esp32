//
// Created by thecave3 on 24/02/20.
//

#ifndef ESP32_BLE_MESH_DEVICE_CALLBACKS_HPP
#define ESP32_BLE_MESH_DEVICE_CALLBACKS_HPP

extern "C" {
#include "core_int.h"
}

/**
 * Callback to be launched after a scan has been completed
 *
 * @param params the device_list array representing the devices found during
 * the scan and the length of the array
 *
 */
void on_scan_completed(bemesh_evt_params_t *params);
void fsm_scan_cmpl(bemesh_evt_params_t *params);

/**
 *
 * @param params
 */
void on_connection_response(bemesh_evt_params_t *params);
void fsm_outgoing_conn_cmpl(bemesh_evt_params_t *params);

/**
 *
 * @param params
 */
void on_incoming_connection(bemesh_evt_params_t *params);
void fsm_incoming_conn_cmpl(bemesh_evt_params_t *params);

/**
 * Callback triggered when a message is received to this device. Parameters to
 * be decided.
 *
 * @param params
 *
 */
void on_message_received(bemesh_evt_params_t *params);
void fsm_msg_recv(bemesh_evt_params_t *params);

/**
 * Callback triggered when disconnection event occurs.
 */
void fsm_disconnect_routine(bemesh_evt_params_t *params);



#endif // ESP32_BLE_MESH_DEVICE_CALLBACKS_HPP
