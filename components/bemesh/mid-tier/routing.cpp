/*
 * routing.cpp
 */

#include "routing.hpp"
#include <memory>
#include <cstring>
#include <algorithm>
#include "bemesh_messages_v2.hpp"

namespace bemesh {
  routing_update_t::routing_update_t(void):params(), update_state() {}
  routing_update_t::routing_update_t(routing_params_t t_params, UpdateState t_state) {
    params=t_params;
    update_state=t_state;
  }

  std::ostream& operator <<(std::ostream& os, const routing_update_t& up) {
    os<<up.params<<up.update_state;
    return os;
  }

  Router::Router(dev_addr_t t_node_addr) : m_rtable(RoutingTable::getInstance()){
    m_node_addr=t_node_addr;
  }

  static bool RoutingParamsCompareFn(routing_params_t& rp1,
				        routing_params_t& rp2) {

    bool rp1_reachable = (rp1.flags&RoutingFlags::Reachable);
    bool rp2_reachable = (rp2.flags&RoutingFlags::Reachable);

    // Reachability: ---------------------------------------------/
    if(rp2_reachable==0) {
      // if new path is not reachable, do not change
      return false;
    }
    if(rp1_reachable == 0) {
      // if new path is feasible and previous isn't, change, if both paths are unfeasible, do not change
      return rp2_reachable == 1;
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
      routing_params_t& old_params= m_rtable.get_routing_params(t_target_addr);
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
      routing_params_t old_params= m_rtable.get_routing_params(t_target_addr);
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

  /**
   * Search an entry, if it exists of a device with bda t_target_addr inside the
   * routing table.
   * 
   * @param t_target_addr bda of the target device begin searched.
   * @return true if the routing table contains the target device, false otherwise.
   */
  bool Router::contains(dev_addr_t t_target_addr) {
    return m_rtable.contains(t_target_addr)==Success;
  }

  /**
   * Extracts the routing parameters for a device with bda t_target_addr inside
   * the routing table.
   * the contains() method should be called first in order to confirm that
   * the target device is present in the routing table. Not doing so, will result
   * in undefined behaviour.
   *
   * @param t_target_addr bda of the target device begin searched.
   * @return reference to the routing params of t_target_addr.
   */
  routing_params_t &Router::get(dev_addr_t t_target_addr) {
    return m_rtable.get_routing_params(t_target_addr);
  }
  
  dev_addr_t& Router::nextHop(dev_addr_t t_target_addr) {
    return m_rtable.get_routing_params(t_target_addr).hop_addr;
  }

  uint8_t Router::targetFlags(dev_addr_t t_target_addr) {
    return m_rtable.get_routing_params(t_target_addr).flags;
  }

  dev_addr_t& Router::addr(void) {
    return m_node_addr;
  }
  std::vector<routing_params_t> Router::getRoutingTable(void) {
    std::vector<routing_params_t> vectorized_rtable=m_rtable.exportTable();
    return vectorized_rtable;
  }

  std::size_t Router::mergeUpdates(std::vector<routing_update_t>& t_update_vect,
				   dev_addr_t t_sender) {
    std::size_t updated_rows=0;
    for(auto const &it : t_update_vect) {
      routing_params_t update_params=it.params;
      UpdateState update_state=(UpdateState)it.update_state;

      if(update_state==UpdateState::Removed) {
	// use the remove method to decide what to do
	if(this->remove(update_params.target_addr)!=UpdateDiscarted) {
	  updated_rows++;
	}
      }
      if(update_state==UpdateState::Added || update_state==UpdateState::Changed) {
	// increase the hop distance by 1
	// and change the hop address to the sender neighbour
	update_params.num_hops+=1;
	update_params.hop_addr=t_sender;
	// use the add method to decide what to do
	if(this->add(update_params)!=UpdateDiscarted) {
	  updated_rows++;
	}
      }
    }
    return updated_rows;
  }

  std::size_t Router::mergeUpdates(std::vector<routing_params_t>& t_params_vect,
			   dev_addr_t t_sender) {
    std::size_t updated_rows=0;
    for(auto &it:t_params_vect) {
      it.num_hops+=1;
      it.hop_addr=t_sender;
      if(this->add(it)!=UpdateDiscarted) {
	updated_rows++;
      }
    }
    return updated_rows;
  }

  // return true if there are updates that must be notified to other nodes.
  // False otherwhise.
  uint8_t Router::hasUpdates(void) {
    return m_update_vect.size()>0;
  }

  // Returns a copy of update_vect update vector. This will clear the internal
  // update_vect in order not to store previously committed updates.
  std::vector<routing_update_t> Router::getRoutingUpdates(void) {
    // Clone the current update_vect in a new vector
    std::vector<routing_update_t> update_vect_copy(m_update_vect);
    // clear the update_vect in order to cancel previous updates
    m_update_vect.clear();
    return update_vect_copy;
  }

  // Returns the neighbours from the routing table
  std::vector<dev_addr_t> Router::getNeighbours(void) {
    std::vector<dev_addr_t> dev_vect;
    std::vector<dev_addr_t>::iterator dev_vect_it;
    std::vector<routing_params_t> vectorized_table=m_rtable.exportTable();

    for(auto &it : vectorized_table) {
      dev_addr_t neighbour=it.hop_addr;
      dev_vect_it=std::find(dev_vect.begin(), dev_vect.end(), neighbour);
      if(dev_vect_it==dev_vect.end()) {
	dev_vect.push_back(neighbour);
      }
    }
    return dev_vect;
  }
  Router &Router::getInstance(dev_addr_t bda) {
    Router instance = Router(bda);
    return instance;
  }

  bool isBroadcast(dev_addr_t& t_addr) {
    return (t_addr==BROADCAST_ADDR);
  }

}

