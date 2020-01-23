/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */

#include "firmware.hpp"

#define PROFILE_A_APP_ID 0
#define PROFILE_B_APP_ID 1

extern "C" {
  void app_main();
}

int main(void) {
  bemesh::main_routine(NULL);
  return 0;
}



void app_main(){
  main();
}
