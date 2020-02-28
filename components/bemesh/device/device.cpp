//
// Created by thecave3 on 15/02/20.
//

#include "device.hpp"

#include "bemesh_messages_v2.hpp"
#include <gatt_def.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_log.h>

// Adding new inclusions.
#include "bemesh_status.hpp"
#include "message_handler_v3.hpp"

using namespace bemesh;

static const char *TAG = "device";

bemesh_dev_t *Device::select_device_to_connect(bemesh_dev_t *device_list,
                                               int length) {
  int i, j;
  bemesh_dev_t temp;

  for (i = 1; i < length; i++) {
    temp = device_list[i];
    j = i - 1;
    while (j >= 0 && device_list[j].rssi > temp.rssi) {
      device_list[j + 1] = device_list[j];
      j--;
    }
    device_list[j + 1] = temp;
  }

  return device_list;
}

void Device::scan_the_environment() {
  kernel_install_cb(ON_SCAN_END, on_scan_completed);
  scan_environment(timeout_sec);
}

void Device::startup() {
  kernel_init();
  scan_the_environment();
}

void Device::server_routine() {
  kernel_install_cb(ON_INC_CONN, on_incoming_connection);
  kernel_install_cb(ON_MSG_RECV, on_message_received);
  start_advertising();

//  while (true) {
//    stop_advertising();
//    this->scan_the_environment();
//    vTaskDelay(timeout_sec / portTICK_PERIOD_MS);
//  }
}

void Device::client_routine() {
  // send a message to every node into the routing table
  ErrStatus ret;
  for (int i = 0; i < this->getRouter().getRoutingTable().size(); i++) {
    RoutingDiscoveryRequest request = RoutingDiscoveryRequest(
        this->getRouter().getRoutingTable()[i].target_addr,
        to_dev_addr(get_own_bda()));
    ret = send_message(&request);
    vTaskDelay(timeout_sec / portTICK_PERIOD_MS);
  }
}
void Device::connect_to_server(esp_bd_addr_t target_server) {
  if (!connect_to(target_server))
    ESP_LOGE(TAG, "Error in connection to server: ");
}

ErrStatus Device::send_message(MessageHeader *message) {
  dev_addr_t &final_dest = message->destination();
  uint8_t *tx_buffer_ptr;
  uint16_t tx_buffer_len;
  ErrStatus ret;
  ret = MessageHandler::getInstance().serialize(message, &tx_buffer_ptr,
                                                &tx_buffer_len);
  if (ret == Success)
    send_payload(this->getRouter().nextHop(final_dest).data(), tx_buffer_ptr,
                 tx_buffer_len);
  return ret;
}

Role Device::getRole() const { return role; }
void Device::setRole(Role newRole) { Device::role = newRole; }
bool Device::isConnected() const { return connected; }
void Device::setConnected(bool newConnected) {
  Device::connected = newConnected;
}
uint8_t Device::getTimeoutSec() const { return timeout_sec; }
void Device::addTimeoutSec(uint8_t timeoutSec) { timeout_sec += timeoutSec; }
void Device::setTimeoutSec(uint8_t timeoutSec) { timeout_sec = timeoutSec; }
Router Device::getRouter() const { return router; }
SemaphoreHandle_t Device::getConnectionSemaphore() const {
  return connectionSemaphore;
}
