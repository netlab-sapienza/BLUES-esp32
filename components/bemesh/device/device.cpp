//
// Created by thecave3 on 15/02/20.
//

#include "device.hpp"

class Device {
private:
  uint_8 timeout_sec = 5;
  /**
   * Order the device_list by putting first the va
   *
   * @param device_list
   * @return
   */
  bemesh_dev_t select_device_to_connect(bemesh_dev_t *device_list) {
    // TODO insertion sort inplace of RSSI, criteria: the higher the better
  }

public:
  Role role;
  void startup() { scan_enviroment(); }

  void server_routine() {}

  void client_routine() {}

  void scan_environment() {
    // scantimeout
  }

  void on_scan_completed(bemesh_dev_t *device_list) {
    bemesh_dev_t target = select_device_to_connect(device_list);
  }
};