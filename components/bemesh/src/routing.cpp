/*
 * routing.cpp
 */

#include "routing.h"
#include <memory>
#include <cstring>

namespace bemesh {
  Router::Router(dev_addr_t t_node_addr):m_rtable() {
    m_node_addr=t_node_addr;
  }

  static bool RoutingParamsCompareFn(routing_params_t& rp1,
				        routing_params_t& rp2) {

    bool rp1_reachable = (rp1.flags&RoutingFlags::Reachable);
    bool rp2_reachable = (rp2.flags&RoutingFlags::Reachable);

    // Reachability: ---------------------------------------------/
    if(rp2_reachable==0) {
      // if new path is not reachable, do not change
      return 0;
    }
    if(rp1_reachable == 0) {
      if(rp2_reachable == 1) {
	// if new path is feasible and previous isnt, change
	return true;
      } else {
	// if both paths are unfeasible, do not change
	return false;
      }
    }
    // Optimality: ---------------------------------------------/
    if(rp1.num_hops > rp2.num_hops) {
      // if old path is longer, change
      return true;
    } else {
      return false;
    }
  }

  ErrStatus Router::add(dev_addr_t t_target_addr, dev_addr_t t_hop_addr,
		       uint8_t t_num_hops, uint8_t t_flags) {
    routing_params_t new_params;
    new_params.target_addr=t_target_addr;
    new_params.hop_addr=t_hop_addr;
    new_params.num_hops=t_num_hops;
    new_params.flags=t_flags;
    
    if(m_rtable.contains(t_target_addr)==Success) {
      // Do we need to update ?
      routing_params_t& old_params=m_rtable.getRoutingParams(t_target_addr);
      if(RoutingParamsCompareFn(old_params, new_params)>0) {
	// new_params is better than old_params, and should be changed
	// replace the params in the routing table
	routing_params_t* old_params_ptr=std::addressof(old_params);
	memcpy(old_params_ptr, &new_params, sizeof(new_params));

	// push the new update in the history update vector
	m_update_vect.push_back(routing_update_t(new_params, UpdateState::Changed));
	return Success;
      } else {
	return UpdateDiscarted;
      }
    } else {
      // t_target_addr is not present, needed to be added
      m_rtable.insert(new_params);
      // push the new update in the history update vector
      m_update_vect.push_back(routing_update_t(new_params, UpdateState::Added));
      return Success;
    }
    return Success;
  }

  ErrStatus Router::add(routing_params_t& t_target_params) {
    return this->add(t_target_params.target_addr,
	      t_target_params.hop_addr,
	      t_target_params.num_hops,
	      t_target_params.flags);
  }

  ErrStatus Router::remove(dev_addr_t t_target_addr) {
    if(m_rtable.contains(t_target_addr)==Success) {
      // generate a stub for the update vector
      routing_params_t old_params=m_rtable.getRoutingParams(t_target_addr);
      routing_params_t stub_params;
      stub_params.target_addr=t_target_addr;
      stub_params.hop_addr=old_params.hop_addr;
      stub_params.num_hops=old_params.num_hops;
      stub_params.flags=old_params.flags;
      // remove the entry from the routing table
      m_rtable.remove(t_target_addr);
      // push the new update in the history update vector
      m_update_vect.push_back(routing_update_t(stub_params, UpdateState::Removed));
      return Success;
    }
    return UpdateDiscarted;
  }

  dev_addr_t& Router::nextHop(dev_addr_t t_target_addr) {
    return m_rtable.getRoutingParams(t_target_addr).hop_addr;
  }

  uint8_t Router::targetFlags(dev_addr_t t_target_addr) {
    return m_rtable.getRoutingParams(t_target_addr).flags;
  }

  std::size_t Router::mergeUpdates(std::vector<routing_update_t>& t_update_vect) {
    std::size_t updated_rows=0;
    for(auto const &it : t_update_vect) {
      routing_params_t update_params=std::get<0>(it);
      UpdateState update_state=std::get<1>(it);

      if(update_state==UpdateState::Removed) {
	// use the remove method to decide what to do
	if(this->remove(update_params.target_addr)!=UpdateDiscarted) {
	  updated_rows++;
	}
      }
      if(update_state==UpdateState::Added || update_state==UpdateState::Changed) {
	// use the add method to decide what to do
	if(this->add(update_params)!=UpdateDiscarted) {
	  updated_rows++;
	}
      }
    }
    return updated_rows;
  }
  
  
}

