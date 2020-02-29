//
// Created by thecave3 on 15/02/20.
//

#include "device.hpp"

extern "C" {
#include <gatt_def.h>
}
#include "bemesh_messages_v2.hpp"
// Adding new inclusions.
#include "bemesh_status.hpp"

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

void Device::start() {

  scan_the_environment();
  ESP_LOGI(TAG, "scan_environment");

  //  server_first_routine();
}

void Device::server_first_routine() {
  kernel_install_cb(ON_MSG_RECV, on_message_received);
  this->server_routine();
}

void Device::server_routine() {
  this->setConnected(true);
  kernel_install_cb(ON_INC_CONN, on_incoming_connection);
  start_advertising();
  vTaskDelay(25000 / portTICK_PERIOD_MS);
  ESP_LOGI(TAG, "start link server server");
  stop_advertising();
  kernel_uninstall_cb(ON_INC_CONN);
  this->setConnected(false);
  ESP_LOGI(TAG, "starting newscan");
  this->scan_the_environment();
}

void Device::client_routine() {
  ESP_LOGI(TAG, "Preparing to launch routing discovery requests.");
  // install message receive callback.
  kernel_install_cb(ON_MSG_RECV, on_message_received);
  // send a message to every node into the routing table
  this->setRole(Role::CLIENT);
  ErrStatus ret;
  std::vector<routing_params_t> rtable = this->getRouter().getRoutingTable();
  for (int i = 0; i < rtable.size(); i++) {
    RoutingDiscoveryRequest request = RoutingDiscoveryRequest(
        rtable[i].target_addr, to_dev_addr(get_own_bda()));
    ESP_LOGI(TAG, "Sending routing discovery request to:");
    ESP_LOG_BUFFER_HEX(TAG, rtable[i].target_addr.data(), 6);

    ret = send_message(&request);
    if (ret != Success) {
      ESP_LOGE(TAG, "Something went wrong on the send message!");
    }
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

Device &Device::getInstance() {
  static Device instance = Device();
  return instance;
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

Device::Device()
    : timeout_sec(5),
      router(bemesh::Router::getInstance(bemesh::to_dev_addr(get_own_bda()))),
      role(Role::UNDEFINED), connected(false),
      connectionSemaphore(xSemaphoreCreateBinary()) {
  ESP_LOGI(TAG, "Nothing to do here...");
}
