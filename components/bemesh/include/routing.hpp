/*
 * routing.h
 * Router should handle everything related to routing issues:
 * - Get the next hop
 * - Handle merge tables
 * - Build update payloads
 */

#pragma once

#include <vector>
#include "bemesh_status.hpp"
#include "rtable.hpp"
#include <ostream>

namespace bemesh {
  enum UpdateState {
    Added=0,
    Changed=1,
    Removed=2
  };

  struct routing_update_t {
    routing_params_t params;
    uint8_t update_state;
    routing_update_t(void);
    routing_update_t(routing_params_t t_params, UpdateState t_state);

    friend std::ostream& operator <<(std::ostream& os, const routing_update_t& up);
  };
  
  struct Router {
    RoutingTable m_rtable;
    // Vector containing updates on the routing table
    std::vector<routing_update_t> m_update_vect;
    // Own node address
    dev_addr_t m_node_addr;

    // Constructor
    Router(dev_addr_t t_node_addr);
    // Insert a new entry in the routing table. Please notice that
    // this function shall be used only for linkage to proximity clients
    ErrStatus add(dev_addr_t t_target_addr, dev_addr_t t_hop_addr,
		 uint8_t t_num_hops, uint8_t t_flags);

    // Insert a new entry in the routing table. This function
    // will do exactly the same things as the one before
    // except that it is used by other internal functions.
    ErrStatus add(routing_params_t& t_target_params);
    // Remove an entry from the routing table. If the removal
    // action goes succesful, a new entry in the update_vect
    // will be added.
    ErrStatus remove(dev_addr_t t_target_addr);

    // Returns the next hop in order to reach t_target_addr
    // if tables were updated succesfully, the hop target
    // should be a neighbour of the current node.
    dev_addr_t& nextHop(dev_addr_t t_target_addr);
    // Returns the flags of t_target_addr node. Please check
    // rtable.h in order to read the flag composition.
    uint8_t targetFlags(dev_addr_t t_target_addr);

    // Returns a constant reference to the router's own address
    dev_addr_t& addr(void);
    // Returns the encoded routing table in form of array of bytes (uint8_t)
    // Used to pass the routing table to other devices.
    std::vector<routing_params_t> getRoutingTable(void);

    // Merge t_update_vect updates in the current routing table. Bear in mind that all the
    // succesful merges generates new entries for the update_vect
    std::size_t mergeUpdates(std::vector<routing_update_t>& t_update_vect,
			     dev_addr_t t_sender);

    std::size_t mergeUpdates(std::vector<routing_params_t>& t_params_vect,
			     dev_addr_t t_sender);

    // return true if there are updates that must be notified to other nodes.
    // False otherwhise.
    uint8_t hasUpdates(void);
    
    // Returns a copy of update_vect update vector. This will clear the internal
    // update_vect in order not to store previously committed updates.
    std::vector<routing_update_t> getRoutingUpdates(void);
  };

}
