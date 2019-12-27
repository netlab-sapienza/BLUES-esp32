/*
 * esp32_ble_mesh.cpp
 * esp32 BeMesh main application file
 */
#include <stdio.h>
#include <vector>

#include "routing.h"
#include "__status.h"

#include "esp_log.h" // for logging
#include "nvs_flash.h" // Non volatile interface
#include "esp_bt.h" // Implements BT controller and VHCI configuration procedures
#include "esp_bt_main.h" // Implements initialization and enabling of Bluedroid stack
#include "esp_gap_ble_api.h" // implements GAP configuration
#include "esp_gatts_api.h" // Implements GATT configuration




static const char* LOG_TAG = "main";
static bemesh::dev_addr_t c0 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static bemesh::dev_addr_t c1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static bemesh::dev_addr_t c2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
static bemesh::dev_addr_t c3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
static bemesh::dev_addr_t c4 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
static bemesh::dev_addr_t c5 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
static bemesh::dev_addr_t c6 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06};


int main(void) {
  uint8_t table_buf[512];
  char buf[256];
  esp_err_t ret;
  bemesh::Router r1(c0);
  r1.add(c1, c1, 0, bemesh::RoutingFlags::Client);
  r1.add(c2, c2, 0, bemesh::RoutingFlags::Client);
  r1.add(c3, c3, 0, bemesh::RoutingFlags::Client);
  r1.add(c4, c4, 0, 0);
  r1.add(c5, c4, 1, bemesh::RoutingFlags::Client);
  r1.add(c6, c4, 1, bemesh::RoutingFlags::Client|bemesh::RoutingFlags::Internet);
  
  printf("routing table size : %d\n",
	 r1.m_rtable.size());
  printf("wish to reach c3, next hop is : ");
  bemesh::printDevAddr(buf, r1.nextHop(c3));
  printf("%s\n", buf);

  std::vector<bemesh::routing_params_t>rtable_vect=r1.m_rtable.exportTable();
  for(auto const& it:rtable_vect) {
    printRoutingParams(it);
  }
  printf("\nPrinting table as array of bytes:\n");
  std::size_t table_size=bemesh::encodeTable(rtable_vect, table_buf, 512);
  for(int i=0;i<table_size;++i) {
    printf("%X", table_buf[i]);
  }
  printf("...done\n");
  
  return 0;
}






extern "C" {
  void app_main();
}
void app_main(){
  main();
}
