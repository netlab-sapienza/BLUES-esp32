/*
 * rtable.cpp
 *
 */

#include "rtable.hpp"
#include <cassert>
#include <cstring>

namespace bemesh {
// Conversion from uint8_t* to dev_addr_t
dev_addr_t to_dev_addr(uint8_t *data) {
  dev_addr_t new_addr;
  memcpy((void *)new_addr.data(), data, DEV_ADDR_LEN);
  return new_addr;
}

int print_dev_addr(char *buf, const dev_addr_t &addr) {
  int wb = 0;
  for (int i = 0; i < DEV_ADDR_LEN; ++i) {
    int ret = sprintf(buf + wb, "%X", addr[i]);
    wb += ret;
  }
  return wb;
}

void print_routing_params(const routing_params_t &p) {
  char buf[256];
  int n = sprintf(buf, "target: ");
  n += print_dev_addr(buf + n, p.target_addr);
  n += sprintf(buf + n, "\thop: ");
  n += print_dev_addr(buf + n, p.hop_addr);
  n += sprintf(buf + n, "\tnum_hops: %d\n", p.num_hops);
  printf("routing params: %s", buf);
}
std::ostream &operator<<(std::ostream &os, const routing_params_t &up) {
  for (auto &it : up.target_addr) {
    os << it;
  }
  for (auto &it : up.hop_addr) {
    os << it;
  }
  os << up.num_hops << up.flags;
  return os;
}

routing_params_t RoutingTable::insert(routing_params_t t_target_params) {
  dev_addr_t target_addr = t_target_params.target_addr;
  m_routing_table.insert(
      std::pair<dev_addr_t, routing_params_t>(target_addr, t_target_params));
  return t_target_params;
}

routing_params_t RoutingTable::insert(dev_addr_t t_target_addr,
                                      dev_addr_t t_hop_addr, uint8_t t_num_hops,
                                      uint8_t t_flags) {
  routing_params_t target_params = {.target_addr = t_target_addr,
                                    .hop_addr = t_hop_addr,
                                    .num_hops = t_num_hops,
                                    .flags = t_flags};
  return insert(target_params);
}

ErrStatus RoutingTable::remove(dev_addr_t t_target_addr) {
  std::map<dev_addr_t, routing_params_t>::iterator it;
  it = m_routing_table.find(t_target_addr);
  assert(it != m_routing_table.end());
  m_routing_table.erase(it);
  return Success;
}

ErrStatus RoutingTable::contains(dev_addr_t &t_target_addr) {
  std::map<dev_addr_t, routing_params_t>::iterator it;
  it = m_routing_table.find(t_target_addr);
  if (it == m_routing_table.end()) {
    return GenericError;
  }
  return Success;
}

routing_params_t &RoutingTable::get_routing_params(dev_addr_t t_target_addr) {
  std::map<dev_addr_t, routing_params_t>::iterator it;
  it = m_routing_table.find(t_target_addr);
  // Stop everything if the routing table contains no routing params for target
  assert(it != m_routing_table.end());
  return it->second;
}

uint16_t RoutingTable::size() { return m_routing_table.size(); }

std::vector<routing_params_t> RoutingTable::exportTable() {
  std::vector<routing_params_t> rtable_vect;
  for (auto const &x : m_routing_table) {
    rtable_vect.push_back(x.second);
  }
  return rtable_vect;
}

int RoutingTable::get_number_of_clients(dev_addr_t t_target_addr) {
  // TODO

  return 0;
}

RoutingTable &RoutingTable::getInstance() {
  static RoutingTable instance;
  return instance;
}
RoutingTable::RoutingTable() : m_routing_table() {}

std::size_t RoutingTable::encodeTable(std::vector<routing_params_t> &t_src_vect,
                        uint8_t *t_dest, std::size_t dest_len) {
  std::size_t exp_byte_size = t_src_vect.size() * sizeof(routing_params_t);
  assert(dest_len >= exp_byte_size);

  memcpy((void *)t_dest, (const void *)t_src_vect.data(), exp_byte_size);
  return exp_byte_size;
}
} // namespace bemesh
