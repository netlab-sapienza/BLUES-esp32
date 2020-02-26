//
// Created by thecave3 on 24/02/20.
//

#include "device_callbacks.hpp"
#include "device.hpp"
#include <esp_log.h>
#include <gatt_def.h>

#define TIMEOUT_DELAY 5
static const char *TAG = "device_callbacks";

void on_scan_completed(bemesh_evt_params_t *params) {
  bemesh_dev_t *device_list = params->scan.result;
  uint16_t list_length = params->scan.len;
  Device &instance = Device::getInstance();

  kernel_uninstall_cb(ON_SCAN_END);

  // This has to be performed only on the first scan. More scan can be launched
  // during the lifecycle of a server
  if (instance.getRole() == Role::UNDEFINED) {
    bemesh_dev_t *target =
        Device::select_device_to_connect(device_list, list_length);
    instance.setRole(Role::CLIENT);

    kernel_install_cb(ON_OUT_CONN, on_connection_response);
    for (int i = 0; !instance.isConnected() && i < list_length;
         i++, *target = device_list[i + 1]) {
      instance.connect_to_server(*target);
      instance.getConnectionSemaphore();
    }
    if (instance.isConnected())
      instance.client_routine();
    else {
      instance.setRole(Role::SERVER);
      instance.addTimeoutSec(TIMEOUT_DELAY);
      instance.server_routine();
    }
  }

  if (instance.getRole() == Role::SERVER) {
    for (int i = 0; i < list_length; i++) {
      if (/*is the device */ device_list[i].bda !=
          nullptr /*in routing table*/) {
        // start merge request
      }
    }
  }
}

void on_connection_response(bemesh_evt_params_t *params) {
  Device instance = Device::getInstance();
  if (params->conn.ack) {
    instance.setConnected(true);
    // sem_post
  } else {
  }
}

void on_incoming_connection(bemesh_evt_params_t *params) {
  Device instance = Device::getInstance();
  auto remote_bda = params->conn.remote_bda;

  if (instance.getRole() == Role::SERVER) {
    if (instance.getRouter().getNeighbours().size() < GATTS_MAX_CONNECTIONS) {
      auto device = bemesh::to_dev_addr((uint8_t *)remote_bda);
      uint8_t t_num_hops = 0;
      uint8_t t_flag = bemesh::Reachable;
      // routing_table.insert(device, device, t_num_hops, t_flag);
    } else {
      // disconnect the device
      // send_message();
    }
  }
}

void on_message_received(bemesh_evt_params_t *params) {
  auto sender = bemesh::to_dev_addr((uint8_t *)params->recv.remote_bda);
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;

  ESP_LOGI(TAG, "a message arrived");
  //    // TODO message reader
  //    bemesh::MessageHandler handler = bemesh::MessageHandler();
  //    handler.read(payload);
  //    handler.handle();

  // if i am the target of the message i'll log it.
  // otherwise i forward the message to the address that the routing table gives
  // me
}
