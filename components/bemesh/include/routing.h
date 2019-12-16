/*
 * routing.h
 * Library that handles routing operations between nodes of the network.
 * Router is an object class which handles routing for the BE-MESH node
 * It's purpose is to handle the routing table, and handle node identifications,
 * redirect messages and so on.
 *
 * The addressing problem is discussed on our paper
 * however a brief introduction is given here:
 * Each address (node_id_t) is composed of two subsections:
 * -server_address (5 bits) : represent the server on which the node is
 *  directly connected
 * -client_address (3 bits) : represent the unique address inside the
 *  server node network
 * It is possible to address 32 servers, each with 7(+1 reserved addr)
 * client nodes.
 * When a client connects to a server, it recives a unique network ID
 * and it may be addressed in the network by using that same ID
 */

#pragma once
#include "__status.h"
#include "__rtable.h"
#include <cstdint>

namespace bemesh {
  typedef uint8_t node_addr_t;

  const uint8_t BROADCAST_ID = 0xFF;
  const uint8_t SERVER_MASK = 0xF8;
  const uint8_t CLIENT_MASK = 0x03;
  
  struct Router {
    // Routing table 
    RoutingTable rtable;
    node_addr_t m_server_addr;
    
    uint8_t m_client_num;
    uint8_t m_server_num;
    
    // Object constructor
    Router(node_addr_t t_server_id);
    // Returns Success if t_client_id node is directly
    // accessible to the server
    // Returns GenericError if more hops must be executed.
    ErrStatus isLocal(node_addr_t t_client_id);
    // Returns the next hop needed to reach t_client_id node
    // In case the Router does not know where to hop,
    // address BROADCAST_ID is returned
    node_addr_t findHop(node_addr_t t_client_id);

    // Add a new connection to the router, that links
    // t_addr node address as t_status status (Server or Client)
    ErrStatus add(node_addr_t t_addr, NodeStatus t_status);

    // Returns the number of directly attached client nodes
    // on this server
    uint8_t clients(void);

    // Returns the number of diectly attached server nodes
    // on this server
    uint8_t servers(void);
  }
}
