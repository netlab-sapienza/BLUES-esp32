/*
 * __rtable.cpp
 * Routing Table
 */

#include "__rtable.h"

namespace bemesh {
  RoutingTable::RoutingTable(void) : m_rtable() {}
  
  std::map<uint8_t, uint8_t> RoutingTable::getTable(void) {
    return m_rtable;
  }
  
  ErrStatus RoutingTable::contains(uint8_t t_client_id) {
    std::map<uint8_t, uint8_t>::iterator it;
    it=m_rtable.find(t_client_id);
    if (it == m_rtable.end()) {
      return GenericError;
    }
    return Success;
  }
  
  uint8_t RoutingTable::getNextHop(uint8_t t_client_id) {
    std::map<uint8_t, uint8_t>::iterator it;
    it=m_rtable.find(t_client_id);
    return it->second;
  }
  
  ErrStatus RoutingTable::insert(uint8_t t_client_id, uint8_t t_next_hop) {
    m_rtable.insert({t_client_id, t_next_hop});
    return Success;
  }

  std::map<uint8_t, uint8_t> update(void) {
    return 
  }
}
