/*
 * routing.cpp
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

#include "routing.h"

namespace bemesh {
  Router::Router(dev_addr_t t_router_addr):m_rtable() {
    m_router_addr=t_router_addr;
    m_last_server_addr=0x00;
  }

  Router(dev_addr_t t_router_addr, node_addr_t t_last_server_addr):m_rtable() {
    m_router_addr=t_router_addr;
    m_last_server_addr=t_last_server_addr;
  }
  // Return Success if t_node_addr node is a client
  // of this router. GenericError otherwise
  ErrStatus Router::checkLoc(node_addr_t t_node_addr) {
    // Check if t_node_addr is in the routing table
    if(m_rtable.contains(t_node_addr)) {
      dev_addr_t hop_dev_addr=m_rtable.getConnParams(t_node_addr).device_addr;
      if(m_router_addr==hop_dev_addr){
	return Success;
      }
    }
    return GenericError;
  }
  // Get the device address needed to reach t_node_addr
  // Notice that checkLoc(t_node_addr) should be called
  // first in order to understand if t_node_addr
  // is directly reachable.
  dev_addr_t Router::nextHop(node_addr_t t_node_addr) {
    if(m_rtable.contains(t_node_addr)) {
      return m_rtable.getConnParams(t_node_addr).device_addr;
    }
    // if not contained... what to do ?
  }
  // Insert a new RoutingConnection in the routing table
  // Returns the added t_node_addr key
  node_addr_t Router::add(node_addr_t t_node_addr,
			  dev_addr_t t_dev_hop,
			  NodeStatus t_is_client,
			  uint8_t t_internet_conn) {
    RoutingConnection conn_params;
    conn_params.device_addr=t_dev_hop;
    conn_params.is_client=t_is_client;
    conn_params.internet_conn=t_internet_conn;
    m_rtable.insert(t_node_addr, conn_params);
    return t_node_addr;    
  }
  // Overloaded method for local client insertion.
  // Automatically generates the node address.
  // Returns the added t_node_addr key.
  node_addr_t Router::add(dev_addr_t t_dev_addr,
			  uint8_t internet_conn) {
    // We need to generate the network address for this new client
    // TODO
  }
  

  /*
  // Object constructor
  Router::Router(node_addr_t t_server_addr):rtable() {
    m_server_addr = t_server_addr;
    m_last_node_idx=0;
    m_client_num=0;
    m_server_num=0;
  }

  // Returns Success if t_client_id node is directly
  // accessible to the server
  // Returns GenericError if more hops must be executed.
  ErrStatus Router::isLocal(node_addr_t t_client_id) {
    // check if t_client_id is in the routing table
    if(rtable.contains(t_client_id)) {
      node_addr_t server_addr = rtable.getNextHop(t_client_id);
      if(server_addr == m_server_addr) {
	return Success;
      }
    }
    return GenericError;
  }
  // Returns the next hop needed to reach t_client_id node
  // In case the Router does not know where to hop,
  // address BROADCAST_ID is returned
  node_addr_t Router::findHop(node_addr_t t_client_id) {
    if(rtable.contains(t_client_id)) {
      node_addr_t server_addr = rtable.getNextHop(t_client_id);
      return server_addr;
    }
    return BROADCAST_ID;
  }

  // Static function needed to add a new node to the Router's table.
  // It outputs the correct network ID for a given server (Router) id and client id
  static node_addr_t _encodeNode(node_addr_t t_server_addr, uint8_t t_last_client) {
    node_addr_t output_addr = 0x00;
    output_addr |= (t_server_addr<<SERVER_MASK_OFFSET)&SERVER_MASK;
    output_addr |= (t_last_client<<CLIENT_MASK_OFFSET)&CLIENT_MASK;
    return output_addr;
  }

  // Add a new connection to the router, that links
  // t_addr node address as t_status status (Server or Client)
  // Notice that t_addr is not related to routing addresses
  // but represent the hardware related static address.
  ErrStatus Router::add(node_addr_t t_addr, NodeStatus t_status) {
    // The network ID is needed for the new device
    // Generate it through _encodeNode function
    node_addr_t node_addr = _encodeNode(m_server_addr, m_last_node_idx++);
    // Include the new pair <node_addr, t_addr> to the routing table.
    rtable.insert(node_addr, t_addr);
    
    if(t_status == Client) {
      m_client_num += 1;
    } else if(t_status == Server) {
      m_server_num += 1;
    }
    return Success;
  }

  // Returns the number of directly attached client nodes
  // on this server
  uint8_t Router::clients(void) {
    return m_client_num;
  }
  
  // Returns the number of diectly attached server nodes
  // on this server
  uint8_t Router::servers(void) {
    return m_server_num;
  }
  */
}
