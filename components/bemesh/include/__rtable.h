/*
 * __rtable.h
 * Routing table
 */

#pragma once
#include <map>
#include <stdint.h>
#include "__status.h"


namespace bemesh {

  struct RoutingConnection;
  //typedef std::map<uint8_t, uint8_t> routing_map_t;
  typedef std::map<uint8_t, RoutingConnection> routing_map_t;

  const uint8_t BT_DEV_ADDR_LEN = 6;

  // Device address used to address devices in the network
  typedef uint8_t dev_addr_t[BT_DEV_ADDR_LEN];
  
  struct RoutingConnection {
    // Each BLE device is addressed through a 48 bit (6 byte)
    // device address
    dev_addr_t device_addr;
    // If we reached a client (or End Point) then notification should
    // happen, otherwise go for message passing
    uint8_t is_client;
    // Flag for internet connection
    uint8_t internet_conn;
  };

  struct RoutingTable{
    // Routing table (map<uint8_t, RoutingConnection>)
    routing_map_t m_rtable;
    // Temporary buffer containing recent updates on the rtable
    // Useful for update notifications
    routing_map_t m_rtable_updates;

    // Object Constructor
    RoutingTable(void);
    // Returns the reference to the current routing table
    const routing_map_t& getTable(void);
    // Returns the reference to the current routing table updates
    const routing_map_t& getUpdateTable(void);
    // Get the Routing Connection params, given the target node id
    const RoutingConnection& getConnParams(uint8_t t_node_id);
    // Return Success if the RoutingTable contains the t_node_id node connection
    ErrStatus contains(uint8_t t_node_id);
    // Insert in the RoutingTable the connection with t_node_id with t_conn_params params
    ErrStatus insert(uint8_t t_node_id, RoutingConnection t_conn_params);
    // Cleans the rtable_updates map. Used after notifying updates
    ErrStatus cleanUpdates(void);  
  };  
}

