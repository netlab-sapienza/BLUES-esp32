/*
 * routing.h
 * Router should handle everything related to routing issues:
 * - Get the next hop
 * - Handle merge tables
 * - Build update payloads
 */

#pragma once

#include "bemesh_status.hpp"
#include "rtable.hpp"
#include <ostream>
#include <vector>

namespace bemesh {
enum UpdateState { Added = 0, Changed = 1, Removed = 2 };

struct routing_update_t {
  routing_params_t params;
  uint8_t update_state;
  routing_update_t();
  routing_update_t(routing_params_t t_params, UpdateState t_state);

  friend std::ostream &operator<<(std::ostream &os, const routing_update_t &up);
};

class Router {
  RoutingTable &m_rtable;
  // Vector containing updates on the routing table
  std::vector<routing_update_t> m_update_vect;
  // Own node address
  dev_addr_t m_node_addr;

  // Constructor
  Router(dev_addr_t t_node_addr);

public:
  /**
   * Insert a new entry in the routing table. Please notice that this function
   * shall be used only for linkage to proximity clients
   *
   * @param t_target_addr
   * @param t_hop_addr
   * @param t_num_hops
   * @param t_flags
   * @return
   */
  ErrStatus add(dev_addr_t t_target_addr, dev_addr_t t_hop_addr,
                uint8_t t_num_hops, uint8_t t_flags);

  /**
   * Insert a new entry in the routing table. This function will do exactly the
   * same things as the one before except that it is used by other internal
   * functions.
   *
   * @param t_target_params
   * @return
   */
  ErrStatus add(routing_params_t &t_target_params);

  /**
   * Remove an entry from the routing table. If the removal action goes
   * successful, a new entry in the update_vect will be added.
   *
   * @param t_target_addr
   * @return
   */
  ErrStatus remove(dev_addr_t t_target_addr);

  /**
   * Returns the next hop in order to reach t_target_addr if tables were updated
   * successfully, the hop target should be a neighbour of the current node.
   * @param t_target_addr
   * @return
   */
  dev_addr_t &nextHop(dev_addr_t t_target_addr);

  /**
   * Search an entry, if it exists of a device with bda t_target_addr inside the
   * routing table.
   *
   * @param t_target_addr bda of the target device begin searched.
   * @return true if the routing table contains the target device, false
   * otherwise.
   */
  bool contains(dev_addr_t t_target_addr);

  /**
   * Extracts the routing parameters for a device with bda t_target_addr inside
   * the routing table.
   * the contains() method should be called first in order to confirm that
   * the target device is present in the routing table. Not doing so, will
   * result in undefined behaviour.
   *
   * @param t_target_addr bda of the target device begin searched.
   * @return reference to the routing params of t_target_addr.
   */
  routing_params_t &get(dev_addr_t t_target_addr);

  /**
   * Returns the flags of t_target_addr node. Please check rtable.h in order to
   * read the flag composition.
   *
   * @param t_target_addr
   * @return
   */
  uint8_t targetFlags(dev_addr_t t_target_addr);

  /**
   * Returns a constant reference to the router's own address
   *
   * @return
   */
  dev_addr_t &addr();

  /**
   * Returns the encoded routing table in form of array of bytes (uint8_t) Used
   * to pass the routing table to other devices.
   * @return
   */
  std::vector<routing_params_t> getRoutingTable();

  /**
   * Merge t_update_vect updates in the current routing table. Bear in mind that
   * all the successful merges generates new entries for the update_vect
   *
   * @param t_update_vect
   * @param t_sender
   * @return
   */
  std::size_t mergeUpdates(std::vector<routing_update_t> &t_update_vect,
                           dev_addr_t t_sender);

  /**
   *
   * @param t_params_vect
   * @param t_sender
   * @return
   */
  std::size_t mergeUpdates(std::vector<routing_params_t> &t_params_vect,
                           dev_addr_t t_sender);

  /**
   * Return true if there are updates that must be notified to other nodes.
   * False otherwise.
   * @return
   */
  uint8_t hasUpdates();

  /**
   * Returns a copy of update_vect update vector. This will clear the internal
   * update_vect in order not to store previously committed updates.
   * @return
   */
  std::vector<routing_update_t> getRoutingUpdates();

  /**
   *  Returns the neighbours from the routing table
   *
   * @return
   */
  std::vector<dev_addr_t> getNeighbours();

  /**
   *
   * @param bda
   * @return
   */
  static Router &getInstance(dev_addr_t bda);

  /**
   * Preprocess the incoming routing table, through a
   * RoutingDiscoveryResponse message, by updating its
   * parameters.
   */
  static void preprocessRoutingTable(dev_addr_t t_remote_bda,
                                     std::vector<routing_params_t> &t_rtable);
};

/**
 *
 * @param t_addr
 * @return
 */
bool isBroadcast(dev_addr_t &t_addr);
} // namespace bemesh
