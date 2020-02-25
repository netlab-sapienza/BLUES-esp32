//
// Created by thecave3 on 15/02/20.
//

#include "device.hpp"
#include <esp_log.h>
#include <gatt_def.h>

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
}

void Device::client_routine() {
  for (int i = 0; i < 100; i++) {
    // send_message();
    vTaskDelay(timeout_sec / portTICK_RATE_MS);
  }
}
void Device::connect_to_server(bemesh_dev_t target_server) {
  connect_to(target_server.bda);
}

void Device::send_message(bemesh_dev_t bda) {
  ESP_LOGI(TAG, "Starting to send a message to ");
  // send_payload(); // TODO bda.nextHop()
}
Role Device::getRole() const { return role; }
void Device::setRole(Role newRole) { Device::role = newRole; }
bool Device::isConnected() const { return connected; }
void Device::setConnected(bool newConnected) {
  Device::connected = newConnected;
}
uint8_t Device::getTimeoutSec() const { return timeout_sec; }
bemesh::Router Device::getRouter() const { return router; }
