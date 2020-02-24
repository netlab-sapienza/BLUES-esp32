//
// Created by thecave3 on 15/02/20.
//

#ifndef ESP32_BLE_MESH_DEVICE_HPP
#define ESP32_BLE_MESH_DEVICE_HPP

#include "core_int.h"
#include "gap_device.h"

// Routing and messages
#include "bemesh_messages_v2.hpp"
#include "message_handler_v2.hpp"

// just for the timeout
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

enum class Role { UNDEFINED = 0, SERVER = 1, CLIENT = 2 };

class Device {
  uint8_t timeout_sec;
  bemesh::Router router;
  Role role;
  bool connected;

  Device()
      : router(
            bemesh::Router::getInstance(bemesh::to_dev_addr(get_own_bda()))) {
    role = Role::UNDEFINED;
    connected = false;
    timeout_sec = 5;
  }

public:
  /**
   * Order function of the scan results  to attempt a connection with the
   * nearest possible (higher RSSI). The order algorithm used is an insertion
   * sort since we expect that the list will be almost completely ordered.
   *
   * @param device_list list of the devices found in the scan
   * @return the first element of the list
   */
  bemesh_dev_t *select_device_to_connect(bemesh_dev_t *device_list,
                                                int length);

  /**
   * Launcher function of the device
   */
  void startup();
  /**
   * Launcher for the scan of the environment
   */
  void scan_the_environment();
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
   * @return true if connected
   */
  bool connect_to_server(bemesh_dev_t target_server);
  /**
   *
   * @param bda
   */
  void send_message(bemesh_dev_t bda);

  /**
   *
   * @return
   */
  static Device &getInstance() {
    Device instance = Device();
    return instance;
  }

  Role getRole() const;
  void setRole(Role newRole);
  bool isConnected() const;
  void setConnected(bool newConnected);
  uint8_t getTimeoutSec() const;
  bemesh::Router getRouter() const;
};

#endif // ESP32_BLE_MESH_DEVICE_HPP
