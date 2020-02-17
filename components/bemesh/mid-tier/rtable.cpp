/*
 * rtable.cpp
 *
 */

#include "rtable.hpp"
#include <cassert>
#include <cstring>

namespace bemesh {

  int printDevAddr(char* buf, const dev_addr_t& addr) {
    int wb=0;
    for(int i=0;i<DEV_ADDR_LEN;++i) {
      int ret=sprintf(buf+wb, "%X", addr[i]);
      wb+=ret;
    }
    return wb;
  }

  void printRoutingParams(const routing_params_t& p) {
    char buf[256];
    int n=sprintf(buf, "target: ");
    n+=printDevAddr(buf+n, p.target_addr);
    n+=sprintf(buf+n, "\thop: ");
    n+=printDevAddr(buf+n, p.hop_addr);
    n+=sprintf(buf+n, "\tnum_hops: %d\n", p.num_hops);
    printf("routing params: %s", buf);
    return;
  }

  RoutingTable::RoutingTable(void): m_routing_table() {

  }

  std::ostream& operator <<(std::ostream& os, const routing_params_t& up) {
    for(auto& it:up.target_addr) {
      os<<it;
    }
    for(auto& it:up.hop_addr) {
      os<<it;
    }
    os<<up.num_hops<<up.flags;
    return os;
  }

  routing_params_t RoutingTable::insert(routing_params_t t_target_params) {
    dev_addr_t target_addr = t_target_params.target_addr;
    m_routing_table.insert(std::pair<dev_addr_t, routing_params_t>(target_addr, t_target_params));
    return t_target_params;
  }

  routing_params_t RoutingTable::insert(dev_addr_t t_target_addr, dev_addr_t t_hop_addr, uint8_t t_num_hops, uint8_t t_flags) {
    routing_params_t target_params;
    target_params.target_addr=t_target_addr;
    target_params.hop_addr=t_hop_addr;
    target_params.num_hops=t_num_hops;
    target_params.flags=t_flags;
    return insert(target_params);
  }

  ErrStatus RoutingTable::remove(dev_addr_t t_target_addr) {
    std::map<dev_addr_t, routing_params_t>::iterator it;
    it=m_routing_table.find(t_target_addr);
    assert(it!=m_routing_table.end());
    m_routing_table.erase(it);
    return Success;
  }

  ErrStatus RoutingTable::contains(dev_addr_t& t_target_addr) {
    std::map<dev_addr_t, routing_params_t>::iterator it;
    it=m_routing_table.find(t_target_addr);
    if(it==m_routing_table.end()) {
      return GenericError;
    }
    return Success;
  }

  routing_params_t& RoutingTable::getRoutingParams(dev_addr_t t_target_addr) {
    std::map<dev_addr_t, routing_params_t>::iterator it;
    it = m_routing_table.find(t_target_addr);
    // Stop everything if the routing table contains no routing params for target
    assert(it != m_routing_table.end());
    return it->second;
  }

  uint16_t RoutingTable::size(void) {
    return m_routing_table.size();
  }

  std::vector<routing_params_t> RoutingTable::exportTable(void) {
    std::vector<routing_params_t> rtable_vect;
    for(auto const& x: m_routing_table) {
      rtable_vect.push_back(x.second);
    }
    return rtable_vect;
  }

  std::size_t encodeTable(std::vector<routing_params_t>& t_src_vect, uint8_t* t_dest, std::size_t dest_len) {
    std::size_t exp_byte_size=t_src_vect.size()*sizeof(routing_params_t);
    assert(dest_len >= exp_byte_size);

    memcpy((void*)t_dest, (const void*)t_src_vect.data(), exp_byte_size);
    return exp_byte_size;
  }
}
