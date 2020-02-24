//
// Created by thecave3 on 24/02/20.
//

#include "device_callbacks.hpp"
#include "device.hpp"
#include <gatt_def.h>

/**
 * Callback to be launched after a scan has been completed
 *
 * @param params the device_list array representing the devices found during
 * the scan and the length of the array
 *
 */
void on_scan_completed(bemesh_evt_params_t *params) {
  bemesh_dev_t *device_list = params->scan.result;
  uint16_t list_length = params->scan.len;
  Device &instance = Device::getInstance();
  bemesh_dev_t *target =
      instance.select_device_to_connect(device_list, list_length);
  instance.setRole(Role::CLIENT);

  for (int i = 0; !instance.isConnected() && i < list_length;
       i++, *target = device_list[i + 1])
    instance.setConnected(connect_to_server(*target));

  if (instance.isConnected())
    instance.client_routine();
  else {
    instance.setRole(Role::SERVER);
    instance.server_routine();
  }
}

void on_connection_response(bemesh_evt_params_t *params) {}

/**
 *
 * @param params
 */
void on_incoming_connection(bemesh_evt_params_t *params) {
  Device instance = Device::getInstance();
  if (instance.getRouter().getNeighbours().size() < GATTS_MAX_CONNECTIONS) {
    auto device = bemesh::to_dev_addr((uint8_t *)params->conn.remote_bda);
    uint8_t t_num_hops = 0;
    uint8_t t_flag = bemesh::Reachable;
    // routing_table.insert(device, device, t_num_hops, t_flag);
  } else {
    // disconnect the device
    // send_message();
  }
}

/**
 * Callback triggered when a message is received to this device. Parameters to
 * be decided.
 *
 * @param params
 *
 */
void on_message_received(bemesh_evt_params_t *params) {
  auto sender = bemesh::to_dev_addr((uint8_t *)params->recv.remote_bda);
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;

  //    // TODO message reader
  //    bemesh::MessageHandler handler = bemesh::MessageHandler();
  //    handler.read(payload);
  //    handler.handle();
}