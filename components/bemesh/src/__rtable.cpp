/*
 * __rtable.cpp
 * Routing Table
 */

#include "__rtable.h"

namespace bemesh {

  RoutingTable::RoutingTable(void) : m_rtable(), m_rtable_updates() {}
  const routing_map_t& RoutingTable::getTable(void) {
    return m_rtable;
  }

  const routing_map_t& RoutingTable::getUpdateTable(void) {
    return m_rtable_updates;
  }
  
  const RoutingConnection& RoutingTable::getConnParams(uint8_t t_node_id) {
    routing_map_t::iterator it;
    it=m_rtable.find(t_node_id);
    return it->second;
  }
  
  ErrStatus RoutingTable::contains(uint8_t t_node_id) {
    routing_map_t::iterator it;
    it=m_rtable.find(t_node_id);
    if(it==m_rtable.end()) {
      return GenericError;
    }
    return Success;
  }
  
  ErrStatus RoutingTable::insert(uint8_t t_node_id, RoutingConnection t_conn_params) {
    m_rtable.insert({t_node_id, t_conn_params});
    m_rtable_updates.insert({t_node_id, t_conn_params});
    return Success;
  }
  
  ErrStatus RoutingTable::cleanUpdates(void) {
    m_rtable_updates.clear();
    return Success;
  }
}
