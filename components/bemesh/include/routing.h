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
  // Network node address (used to address nodes
  // inside the network)
  typedef uint8_t node_addr_t;

  const uint8_t BROADCAST_ID = 0xFF;
  const uint8_t SERVER_MASK_OFFSET = 0x03;
  const uint8_t CLIENT_MASK_OFFSET = 0x00;
  const uint8_t SERVER_MASK = 0xF8;
  const uint8_t CLIENT_MASK = 0x03;

  struct Router {
    RoutingTable m_rtable;

    // Last server address in the piconet
    // If a new server connects to this router,
    // he shall assign himself this address
    node_addr_t m_last_server_addr;
    dev_addr_t m_router_addr;
    Router(dev_addr_t t_router_addr);
    Router(dev_addr_t t_router_addr, node_addr_t t_last_server_addr);
    // Return Success if t_node_addr node is a client
    // of this router. GenericError otherwise
    ErrStatus checkLoc(node_addr_t t_node_addr);
    // Get the device address needed to reach t_node_addr
    // Notice that checkLoc(t_node_addr) should be called
    // first in order to understand if t_node_addr
    // is directly reachable.
    dev_addr_t nextHop(node_addr_t t_node_addr);
    // Insert a new RoutingConnection in the routing table
    // Returns the added t_node_addr key
    node_addr_t add(node_addr_t t_node_addr,
		  dev_addr_t t_dev_hop,
		  NodeStatus t_is_client,
		  uint8_t internet_conn);
    // Overloaded method for local client insertion.
    // Automatically generates the node address.
    // Returns the added t_node_addr key.
    node_addr_t add(dev_addr_t t_dev_addr,
		  uint8_t internet_conn);
  }

  

  /*
  struct Router {
    // Routing table 
    RoutingTable rtable;
    node_addr_t m_server_addr;

    uint8_t m_last_node_idx;
    uint8_t m_client_num;
    uint8_t m_server_num;
    
    // Object constructor
    Router(node_addr_t t_server_addr);
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
    // Notice that t_addr is not related to routing addresses
    // but represent the hardware related static address.
    ErrStatus add(node_addr_t t_addr, NodeStatus t_status);

    // Returns the number of directly attached client nodes
    // on this server
    uint8_t clients(void);

    // Returns the number of diectly attached server nodes
    // on this server
    uint8_t servers(void);
  };
  */
}
