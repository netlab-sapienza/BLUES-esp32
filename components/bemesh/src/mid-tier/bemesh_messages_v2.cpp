/*
 * bemesh_messages_v2.cpp
 * its bemesh_messages but messages are treated like classes
 * as they should
 */

#include "bemesh_messages_v2.hpp"

namespace bemesh {
  MessageHeader::MessageHeader(dev_addr_t t_dest, dev_addr_t t_src, uint8_t t_id,
			       uint8_t t_hops, uint8_t t_seq, uint8_t t_psize):
    m_dest_addr(t_dest),m_src_addr(t_src),m_id(t_id),m_hops(t_hops),m_seq(t_seq),
    m_psize(t_psize) {}

  dev_addr_t& MessageHeader::destination(void) {
    return m_dest_addr;
  }
  
  dev_addr_t& MessageHeader::source(void) {
    return m_src_addr;
  }
  
  uint8_t MessageHeader::id(void) {
    return m_id;
  }
  
  uint8_t MessageHeader::hops(void) {
    return m_hops;
  }
  
  uint8_t MessageHeader::seq(void) {
    return m_seq;
  }
  
  uint8_t MessageHeader::psize(void) {
    return m_psize;
  }
  
  void MessageHeader::setHops(uint8_t t_hops) {
    m_hops=t_hops;
  }

  std::size_t MessageHeader::size(void) {
    return sizeof(MessageHeader)+m_psize;
  }
  // Notice the t_psize+1 payload size for indexedMessages (+1 accounts for m_entries val in the payload)
  IndexedMessage::IndexedMessage(std::size_t t_entries, dev_addr_t t_dest, dev_addr_t t_src, uint8_t t_id,
				 uint8_t t_hops, uint8_t t_seq, uint8_t t_psize):
    MessageHeader(t_dest, t_src, t_id, t_hops, t_seq, t_psize+1), m_entries(t_entries) {}
  
  std::size_t IndexedMessage::entries(void) {
    return m_entries;
  }

  RoutingDiscoveryRequest::RoutingDiscoveryRequest(dev_addr_t t_dest, dev_addr_t t_src):
    MessageHeader(t_dest, t_src, ROUTING_DISCOVERY_REQ_ID, 0, 0, 0){}

  RoutingDiscoveryResponse::RoutingDiscoveryResponse(dev_addr_t t_dest, dev_addr_t t_src,
						     std::array<routing_params_t,
						     ROUTING_DISCOVERY_RES_ENTRIES_MAX> t_payload,
						     std::size_t t_pentries):
    IndexedMessage(t_pentries, t_dest, t_src, ROUTING_DISCOVERY_RES_ID, 0, 0, 0),
    m_payload(t_payload){
    // Update the payload size based on t_payload used size
    m_psize+=sizeof(routing_params_t)*t_pentries;
  }

  std::array<routing_params_t, ROUTING_DISCOVERY_RES_ENTRIES_MAX> RoutingDiscoveryResponse::payload(void) {
    return m_payload;
  }

  RoutingUpdateMessage::RoutingUpdateMessage(dev_addr_t t_dest, dev_addr_t t_src,
					     std::array<routing_update_t,
					     ROUTING_UPDATE_ENTRIES_MAX> t_payload,
					     std::size_t t_pentries):
  IndexedMessage(t_pentries, t_dest, t_src, ROUTING_UPDATE_ID, 0, 0, 0),
    m_payload(t_payload) {
    // Update the payload size based on t_payload used size
    m_psize+=sizeof(routing_update_t)*t_pentries;
  }

  std::array<routing_update_t, ROUTING_UPDATE_ENTRIES_MAX> RoutingUpdateMessage::payload(void) {
    return m_payload;
  }
  
}
