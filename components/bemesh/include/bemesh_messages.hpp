/*
 * bemesh_messages.h
 */

#pragma once

#include "rtable.hpp"
#include "routing.hpp"
#include "message_constant.hpp"



//TODO: fonte di possibili problemi: carattere di terminazione.

#define MAX_COMM_MESSAGE_LENGHT 210  //255 -17 -18 



namespace bemesh {
  // Number of possible different messages
  #define MESSAGE_TYPES_MAX 16

  #define MESSAGE_SIZE_MAX 255

  /*
   * Header structure:
   * - destination [dev_addr] 6
   * - source [dev_addr]      6
   * - seq [uint8]            1
   * - hops [uint8]           1
   * - message id [uint8]     1
   * - payload_size [uint8]   1

   * -- TOTAL : 16 bytes
   */
  struct MessageHeader {
    dev_addr_t dest_addr;
    dev_addr_t src_addr;
    uint8_t message_id;
    uint8_t hops;
    uint8_t seq;
    uint8_t payload_size;
  };

  inline std::size_t MESSAGE_SIZE(MessageHeader* h) {return (sizeof(MessageHeader)+h->payload_size);}
  struct NumeratedMessageHeader {
    MessageHeader header; // 16 bytes
    uint8_t num_entries; // 1 byte = 17 bytes
  };

  struct RoutingDiscoveryRequest {
    MessageHeader header; // 16 bytes
  };
#define ROUTING_DISCOVERY_REQ_ID 0x01

  struct RoutingDiscoveryResponse {
    NumeratedMessageHeader header; // 17 bytes
#define MAX_ROUTING_PARAMS_RESP 16
    // 14 bytes per entry
    routing_params_t r_params[MAX_ROUTING_PARAMS_RESP];
    // A GATT Message may cntain a PDU with max size 255
  };
#define ROUTING_DISCOVERY_RES_ID 0x02

  struct RoutingUpdate {
    NumeratedMessageHeader header;
#define MAX_ROUTING_UPDATE_ENTRIES 13
    routing_update_t r_updates[MAX_ROUTING_UPDATE_ENTRIES];
  };
#define ROUTING_UPDATE_ID 0x03
  
  struct CommunicationMessage{
    //header part
    NumeratedMessageHeader header; //17 bytes
    uint8_t operation; //18 bytes

    //payload part (check it is less than 255 bytes)
    char payload[MAX_COMM_MESSAGE_LENGHT];
  };
#define COMMUNICATION_MESSAGE_ID 0x04

}
