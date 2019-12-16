/*
 * __rtable.h
 * Routing table
 */

#pragma once
#include <map>
#include <stdint.h>
#include "__status.h"


namespace bemesh {
  typedef std::map<uint8_t, uint8_t> routing_map_t;
  
  struct RoutingTable{
    // Routing table as a map<uint8_t, uint8_t>
    routing_map_t m_rtable;
    // Routing table recent updates
    routing_map_t m_rtable_updates;
    
    // Object constructor
    RoutingTable(void);
    // Returns a read only reference to the internal
    // routing table
    const routing_map_t & getTable(void);
    // Check if t_client_id key is present in m_rtable
    ErrStatus contains(uint8_t t_client_id);
    // Returns the value related to t_client_id key
    uint8_t getNextHop(uint8_t t_client_id);
    // Insert a new <key, value> addition to m_rtable
    // If t_client_id key is already found in the map,
    // the value will not be updated
    ErrStatus insert(uint8_t t_client_id, uint8_t t_next_hop);
    // Returns the m_rtable_updates map containing recent
    // additions to the table. Used to reduce the payload of
    // update routing table messages.
    const routing_map_t& update_table(void);
    // Placeholder for cleaning m_rtable_updates map
    ErrStatus update(void);
  };
}

