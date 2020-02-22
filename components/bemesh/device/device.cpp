//
// Created by thecave3 on 15/02/20.
//

#include "device.hpp"
#include <gatt_def.h>

class Device {
private:
  uint8_t timeout_sec = 5;
  bemesh::Router router;

  /**
   * Order function of the scan results  to attempt a connection with the
   * nearest possible (higher RSSI). The order algorithm used is an insertion
   * sort since we expect that the list will be almost completely ordered.
   *
   * @param device_list list of the devices found in the scan
   * @return the first element of the list
   */
  static bemesh_dev_t *select_device_to_connect(bemesh_dev_t *device_list,
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

  /**
   * Tries to connect to a server in order to become a client of that specific
   * server
   *
   * @param target_server device representing the active server previously
   * discovered
   * @return true if connected
   */
  static bool connect_to_server(bemesh_dev_t target_server) {
    return (bool)!connect_to(target_server.bda);
  }

  /**
   *
   * @param bda
   */
  void send_message(bemesh_dev_t bda) {

    //send_payload(); // TODO bda.nextHop()
  }

public:
  Role role;
  bool connected;

  Device()
      : router(
            bemesh::Router::getInstance(bemesh::to_dev_addr(get_own_bda()))) {
    role = Role::UNDEFINED;
    connected = false;
  }

  /**
   * Launcher function of the device
   */
  void startup() {
    kernel_init();
    scan_environment(timeout_sec);
  }

  /**
   * Operations performed as a server;
   */
  void server_routine() {}

  /**
   * Operations performed as a client;
   *
   * In this case the client is stupid, every timeout_sec seconds it sends a
   * message to a random device into its routing table
   *
   *
   */
  void client_routine() {
    for (int i = 0; i < 100; i++) {
      send_message();
      vTaskDelay(timeout_sec / portTICK_RATE_MS);
    }
  }

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

    bemesh_dev_t *target = select_device_to_connect(device_list, list_length);
    this->role = Role::CLIENT;

    for (int i = 0; !connected && i < list_length;
         i++, *target = device_list[i + 1])
      connected = connect_to_server(*target);

    if (connected)
      client_routine();
    else {
      this->role = Role::SERVER;
      server_routine();
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
    esp_bd_addr_t *sender = params->recv.remote_bda;
    uint8_t *payload = params->recv.payload;
    uint16_t payload_len = params->recv.len;

    // TODO message reader
    bemesh::MessageHandler handler = bemesh::MessageHandler();
    handler.read(payload);
    handler.handle();
  }

  /**
   *
   * @param params
   */
  void on_incoming_connection(bemesh_evt_params_t *params) {
    if (router.getNeighbours().size() < GATTS_MAX_CONNECTIONS) {
      auto device = bemesh::to_dev_addr((uint8_t *)params->conn.remote_bda);
      uint8_t t_num_hops = 0;
      uint8_t t_flag = bemesh::Reachable;
    // routing_table.insert(device, device, t_num_hops, t_flag);
    } else {
      // disconnect the device
      send_message();
    }
  }
};