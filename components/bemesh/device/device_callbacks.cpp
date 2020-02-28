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

  // This has to be performed only on the first scan. More scan can be launched
  // during the lifecycle of a server
  if (instance.getRole() == Role::UNDEFINED) {
    if (list_length > 0) {
      ESP_LOGI(TAG, "scancmpl: executing undefined role routine.");
      bemesh_dev_t *target =
	Device::select_device_to_connect(device_list, list_length);
      // debug infos.
      ESP_LOGI(TAG, "Choosen server with rssi:%d and bda:", target->rssi);
      ESP_LOG_BUFFER_HEX(TAG, target->bda, ESP_BD_ADDR_LEN);
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
      ESP_LOGI(TAG, "Starting client routine...");
      // TODO(Andrea): Set the client role where you please.
      instance.setRole(Role::CLIENT);
      // Add the new entry to the routing table
      ESP_LOGI(TAG, "Adding to routing table the new entry:");
      ESP_LOG_BUFFER_HEX(TAG, conn_target.bda, ESP_BD_ADDR_LEN);
      auto device = to_dev_addr(conn_target.bda);
      ESP_LOG_BUFFER_HEX(TAG, device.data(), ESP_BD_ADDR_LEN);
      instance.getRouter().add(device, device, 0, Reachable);
      // Launch client routine
      instance.client_routine();
    }
    else {
      ESP_LOGI(TAG, "Could not connect to server. Initializing server routine.");
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
  ESP_LOGI(TAG, "start incoming connection");
  Device instance = Device::getInstance();
  ESP_LOGI(TAG, "start incoming connection");
  auto remote_bda = params->conn.remote_bda;
  auto device = to_dev_addr((uint8_t *)remote_bda);
  uint8_t t_num_hops = 0;
  uint8_t t_flag = Reachable;
  ESP_LOGI(TAG, "add device to router");
  instance.getRouter().add(device, device, t_num_hops, t_flag);
  ESP_LOGI(TAG, "device added");

  if (get_num_inc_conn() < GATTS_MAX_CONNECTIONS)
    start_advertising();
}

void on_message_received(bemesh_evt_params_t *params) {
  auto sender = to_dev_addr((uint8_t *)params->recv.remote_bda);
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;
  Device instance = Device::getInstance();
  MessageHandler handler = MessageHandler::getInstance();

  if (instance.getRole() == Role::SERVER) {
    MessageHeader *message = handler.unserialize(payload, payload_len);
    switch (message->id()) {
    case ROUTING_DISCOVERY_REQ_ID: {
      std::vector<routing_params_t> routing_table =
          instance.getRouter().getRoutingTable();
      RoutingDiscoveryResponse response = RoutingDiscoveryResponse(
          message->source(), to_dev_addr(get_own_bda()), routing_table,
          routing_table.size());
      instance.send_message(&response);
      break;
    }
    case ROUTING_DISCOVERY_RES_ID: {
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
  }

  // if i am the target of the message i'll log it.
  // otherwise i forward the message to the address that the routing table gives
  // me
}
