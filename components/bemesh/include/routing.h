/*
 * routing.h
 * Router should handle everything related to routing issues:
 * - Get the next hop
 * - Handle merge tables
 * - Build update payloads
 */

#pragma once

#include <vector>
#include "__status.h"
#include "rtable.h"

namespace bemesh {
  enum UpdateState {
    Added=0,
    Changed=1,
    Removed=2
  };

  struct routing_update_t {
    routing_params_t params;
    UpdateState update_state;

    routing_update_t(routing_params_t t_params, UpdateState t_state);
  };
  
  struct Router {
    RoutingTable m_rtable;
    // Vector containing updates on the routing table
    std::vector<routing_update_t> m_update_vect;
    // Own node address
    dev_addr_t m_node_addr;
    
    Router(dev_addr_t t_node_addr);
    ErrStatus add(dev_addr_t t_target_addr, dev_addr_t t_hop_addr,
		 uint8_t t_num_hops, uint8_t t_flags);

    ErrStatus add(routing_params_t& t_target_params);
    ErrStatus remove(dev_addr_t t_target_addr);

    dev_addr_t& nextHop(dev_addr_t t_target_addr);
    uint8_t targetFlags(dev_addr_t t_target_addr);
    
    std::size_t mergeUpdates(std::vector<routing_update_t>& t_update_vect);    
  };

}
