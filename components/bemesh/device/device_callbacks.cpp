//
// Created by thecave3 on 24/02/20.
//

#include "device_callbacks.hpp"
#include "device.hpp"
#include <cstring>

extern "C" {
#include <esp_log.h>
#include <gatt_def.h>
}

#define TIMEOUT_DELAY 10
static const char *TAG = "device_callbacks";

using namespace bemesh;

void on_scan_completed(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Starting OnScanComplete operation.");
  bemesh_dev_t *device_list = params->scan.result;
  uint16_t list_length = params->scan.len;
  Device &instance = Device::getInstance();

  // connection target which will be copied from the params
  // used to add an entry in the routing table.
  bemesh_dev_t conn_target;

  kernel_uninstall_cb(ON_SCAN_END);

  // TODO check if there're new devices
  // if these devices are not in the routing table then i have to merge with them



  if (list_length > 0) {
    bemesh_dev_t *target =
        Device::select_device_to_connect(device_list, list_length);
    ESP_LOGI(TAG, "onscancmpl: starting undefined routine.");
    // copy the target in the conn_target support object.
    memcpy(&conn_target, target, sizeof(bemesh_dev_t));
    kernel_install_cb(ON_OUT_CONN, on_connection_response);
    for (int i = 0; !instance.isConnected() && i < list_length;
         i++, *target = device_list[i + 1]) {
      ESP_LOGI(TAG, "Attempt to connect to server.");
      instance.connect_to_server(target->bda);
      ESP_LOGI(TAG, "Locking the connection semaphore.");
      xSemaphoreTake(instance.getConnectionSemaphore(), portMAX_DELAY);
      ESP_LOGI(TAG, "Semaphore unlocked.");
    }
  }

  if (instance.isConnected()) {
    ESP_LOGI(TAG, "onscancmpl: starting client routine.");
    // Add the new entry to the routing table
    auto device = to_dev_addr(conn_target.bda);
    instance.getRouter().add(device, device, 0, Reachable);
    if (instance.getRole() == Role::UNDEFINED) {
      // Launch client routine
      instance.client_routine();
    }
    if (instance.getRole() == Role::SERVER) {
      // Link Server <-> Server
      RoutingDiscoveryRequest request =
          RoutingDiscoveryRequest(device, to_dev_addr(get_own_bda()));
      ESP_LOGI(TAG, "Sending routing discovery request to:");
      ESP_LOG_BUFFER_HEX(TAG, device.data(), 6);

      ErrStatus ret = instance.send_message(&request);
      if (ret != Success) {
        ESP_LOGE(TAG, "Something went wrong on the send message!");
      }
    }
  } else {
    ESP_LOGI(TAG, "onscancmpl: starting server routine.");
    instance.addTimeoutSec(TIMEOUT_DELAY);
    if (instance.getRole() == Role::UNDEFINED) {
      instance.setRole(Role::SERVER);
      instance.server_first_routine();
    } else {
      // i am already a server i do not need to instantiate the callbacks
      // again
      instance.server_routine();
    }
  }
}

void on_connection_response(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Connection response callback.");
  Device &instance = Device::getInstance();

  if (instance.getRole() == Role::UNDEFINED) {
    ESP_LOGI(TAG, "Starting undefined role routine.");
    if (params->conn.ack) {
      ESP_LOGI(TAG, "Setting connected flag to true.");
      instance.setConnected(true);
    }
  } else if (instance.getRole() == Role::SERVER) {
    ESP_LOGI(TAG, "Starting server role routine.");
    RoutingDiscoveryRequest request = RoutingDiscoveryRequest(
        to_dev_addr(params->conn.remote_bda), to_dev_addr(get_own_bda()));
    ESP_LOGI(TAG, "Sending routing discovery request.");
    instance.send_message(&request);
  }
  xSemaphoreGive(instance.getConnectionSemaphore());
}

void on_incoming_connection(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Incoming connection callback.");
  Device instance = Device::getInstance();
  auto remote_bda = params->conn.remote_bda;
  auto device = to_dev_addr((uint8_t *)remote_bda);
  uint8_t t_num_hops = 0;
  uint8_t t_flag = Reachable;
  instance.getRouter().add(device, device, t_num_hops, t_flag);

  if (get_num_inc_conn() < GATTS_MAX_CONNECTIONS)
    start_advertising();
}

void on_message_received(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Message received callback.");
  //  auto sender = to_dev_addr((uint8_t *)params->recv.remote_bda);
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;
  Device instance = Device::getInstance();
  MessageHandler handler = MessageHandler::getInstance();
  MessageHeader *message = handler.unserialize(payload, payload_len);
  // DEBUG ONLY (Print the message header)
  // ESP_LOGI(TAG, "@@@ Printing received message header @@@");
  // ESP_LOGI(TAG, "Destination:");
  // ESP_LOG_BUFFER_HEX(TAG, message->destination().data(), ESP_BD_ADDR_LEN);
  // ESP_LOGI(TAG, "Source:");
  // ESP_LOG_BUFFER_HEX(TAG, message->source().data(), ESP_BD_ADDR_LEN);
  // ESP_LOGI(TAG, "Payload size: %d", message->psize());

  if (message->destination() != to_dev_addr(get_own_bda())) {
    ESP_LOGI(TAG, "This message is not for me.");
  }
  switch (message->id()) {
  case ROUTING_DISCOVERY_REQ_ID: {
    if (instance.getRole() == Role::SERVER) {
      ESP_LOGI(TAG, "Received RoutingDiscoveryRequest");
      std::vector<routing_params_t> routing_table =
          instance.getRouter().getRoutingTable();
      RoutingDiscoveryResponse response = RoutingDiscoveryResponse(
          message->source(), to_dev_addr(get_own_bda()), routing_table,
          routing_table.size());
      // ESP_LOGI(TAG, "@@@ Printing outgoing message header @@@");
      // ESP_LOGI(TAG, "Destination:");
      // ESP_LOG_BUFFER_HEX(TAG, response.destination().data(),
      // ESP_BD_ADDR_LEN); ESP_LOGI(TAG, "Source:"); ESP_LOG_BUFFER_HEX(TAG,
      // response.source().data(), ESP_BD_ADDR_LEN); ESP_LOGI(TAG, "Payload
      // size: %d", response.psize()); ESP_LOGI(TAG, "Preparing to send
      // response.");
      instance.send_message(&response);
    }
    break;
  }
  case ROUTING_DISCOVERY_RES_ID: {
    ESP_LOGI(TAG, "Received routing discovery response");
    ESP_LOG_BUFFER_HEX(TAG, message->source().data(), ESP_BD_ADDR_LEN);
    auto *res_packet = (RoutingDiscoveryResponse *)message;
    for (int i = 0; i < res_packet->entries(); ++i) {
      auto &entry = res_packet->payload()[i];
      instance.getRouter().add(entry);
    }
    break;
  }
  case ROUTING_PING_ID: {
    ESP_LOGI(TAG, "ping received");
    break;
  }
  case ROUTING_SYNC_ID: {
    ESP_LOGI(TAG, "routing sync id");
    break;
  }
  case ROUTING_UPDATE_ID: {
    ESP_LOGI(TAG, "update id");
    break;
  }
  default: {
    ESP_LOGE(TAG, "Cannot identify message");
  }
  }

  // if i am the target of the message i'll log it.
  // otherwise i forward the message to the address that the routing table gives
  // me
}

// TODO(Andrea): Remember your gf's friend <3
