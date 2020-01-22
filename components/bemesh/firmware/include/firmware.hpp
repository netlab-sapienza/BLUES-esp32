/*
 * firmware.hpp
 * Main routines to be executed on the esp32 board
 */

#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>

// mid-tier libraries
#include "slave.hpp"
#include "master.hpp"
#include "callbacks.hpp"
#include "bemesh_error.hpp"

extern "C" {
#include "esp_log.h"
}

namespace bemesh {
  void main_routine(void* args);
}
