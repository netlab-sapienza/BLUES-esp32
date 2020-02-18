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


class Device;

#endif // ESP32_BLE_MESH_DEVICE_HPP
