//
// Created by thecave3 on 24/02/20.
//

#include "device_callbacks.hpp"
#include "device.hpp"
#include <esp_log.h>
#include <gatt_def.h>

#define TIMEOUT_DELAY 5
static const char *TAG = "device_callbacks";

using namespace bemesh;

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
      instance.connect_to_server(target->bda);
      xSemaphoreTake(instance.getConnectionSemaphore(), 0);
    }
    if (instance.isConnected())
      instance.client_routine();
    else {
      instance.setRole(Role::SERVER);
      instance.addTimeoutSec(TIMEOUT_DELAY);
      instance.server_routine();
    }
  }

  // This is the server behaviour when is already up.
  //
  if (instance.getRole() == Role::SERVER) {
    for (int i = 0; i < list_length; i++) {
      if (!instance.getRouter().contains(to_dev_addr(device_list[i].bda))) {
        instance.connect_to_server(device_list->bda);
      }
    }
  }
}

void on_connection_response(bemesh_evt_params_t *params) {
  Device instance = Device::getInstance();

  if (instance.getRole() == Role::UNDEFINED) {
    if (params->conn.ack) {
      instance.setConnected(true);
    }
  } else if (instance.getRole() == Role::SERVER) {
    RoutingDiscoveryRequest request = RoutingDiscoveryRequest(
        to_dev_addr(params->conn.remote_bda), to_dev_addr(get_own_bda()));
    instance.send_message(&request);
  }
  xSemaphoreGive(instance.getConnectionSemaphore());
}

void on_incoming_connection(bemesh_evt_params_t *params) {
  Device instance = Device::getInstance();
  auto remote_bda = params->conn.remote_bda;

  if (instance.getRole() == Role::SERVER) {
    if (instance.getRouter().getNeighbours().size() < GATTS_MAX_CONNECTIONS) {
      auto device = to_dev_addr((uint8_t *)remote_bda);
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
  auto sender = to_dev_addr((uint8_t *)params->recv.remote_bda);
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;
  Device instance = Device::getInstance();

  if (instance.getRole() == Role::SERVER) {
    MessageHeader *message =
        MessageHandler::getInstance().unserialize(payload, payload_len);
    switch (message->id()) {
    case ROUTING_DISCOVERY_REQ_ID:
      std::vector<routing_params_t> routing_table =
          instance.getRouter().getRoutingTable();
      RoutingDiscoveryResponse response = RoutingDiscoveryResponse(
          message->source(), to_dev_addr(get_own_bda()), routing_table,
          routing_table.size());
      break;
    case ROUTING_DISCOVERY_RES_ID:
      break;
    case ROUTING_PING_ID:
      break;
    case ROUTING_SYNC_ID:
      break;
    case ROUTING_UPDATE_ID:
      break;
    default:
      ESP_LOGE(TAG, "Cannot identify message");
    }
  }

  // if i am the target of the message i'll log it.
  // otherwise i forward the message to the address that the routing table gives
  // me
}
