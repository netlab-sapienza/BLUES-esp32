//
// Created by thecave3 on 15/02/20.
//

#ifndef ESP32_BLE_MESH_DEVICE_HPP
#define ESP32_BLE_MESH_DEVICE_HPP

extern "C" {
#include "core_int.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "gap_device.h"
#include <esp_log.h>
}

// Routing and messages
#include "bemesh_messages_v2.hpp"
#include "message_handler_v3.hpp"

// just for the timeout
#include "device_callbacks.hpp"

enum class Role { UNDEFINED = 0, SERVER = 1, CLIENT = 2 };

enum DeviceState {
  Uninitialized=0,
  Scanning=1,
  Advertising=2,
  Connecting=3,
  RTClientSentReq=4, // RoutingDiscovery : Client sent request
  RTServerRecvReq=5, // RoutingDiscovery : Server recv request
  RTFinished=6,      // RoutingDiscovery : Procedure Finished.
  //TODO(Emanuele, Andrea): Add extra states
};

// Timeout for the advertising procedure in millisecond (ms)
#define DEVICE_TIMEOUT_ADV_MS 5000
// Timeout for the scanning procedure in millisecond (ms)
#define DEVICE_TIMEOUT_SCN_S 5

class Device {
  uint8_t timeout_sec;
  uint16_t adv_timeout_sec; // Advertising timeout
  uint16_t scn_timeout_sec; // Scanning timeout
  bemesh::Router &router;
  Role role;
  bool connected;
  SemaphoreHandle_t connectionSemaphore;

  // variable to represent the current device's state.
  DeviceState m_state;
  Device();

public:
  /**
   * Order function of the scan results  to attempt a connection with the
   * nearest possible (higher RSSI). The order algorithm used is an insertion
   * sort since we expect that the list will be almost completely ordered.
   *
   * @param device_list list of the devices found in the scan
   * @return the first element of the list
   */
  static bemesh_dev_t *select_device_to_connect(bemesh_dev_t *device_list,
                                                int length);

  /**
   * Launcher function of the device
   */
  void start();
  /**
   * Launcher for the scan of the environment
   */
  void scan_the_environment();
  /**
   * Operations performed as a server only the first time;
   */
  void server_first_routine();

  /**
   * Operations performed as a server;
   */
  void server_routine();

  /**
   * Operations performed as a client;
   *
   * In this case the client is stupid, every timeout_sec seconds it sends a
   * message to a random device into its routing table
   *
   *
   */
  void client_routine();

  /**
   * Tries to connect to a server in order to become a client of that specific
   * server
   *
   * @param target_server device representing the active server previously
   * discovered
   */
  void connect_to_server(esp_bd_addr_t target_server);

  /**
   *
   * @param bda
   */
  bemesh::ErrStatus send_message(bemesh::MessageHeader *message);

  /**
   *
   * @return
   */
  static Device &getInstance();

  Role getRole() const;
  void setRole(Role newRole);
  DeviceState getState() const;
  void setState(DeviceState t_state);
  bool isConnected() const;
  void setConnected(bool newConnected);
  uint8_t getTimeoutSec() const;
  uint16_t getAdvTimeout() const;
  uint16_t getScnTimeout() const;
  bemesh::Router getRouter() const;
  void addTimeoutSec(uint8_t timeoutSec);
  void setTimeoutSec(uint8_t timeoutSec);
  SemaphoreHandle_t getConnectionSemaphore() const;
};

#endif // ESP32_BLE_MESH_DEVICE_HPP
