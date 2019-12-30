/*
 * bemesh_messages_v2.cpp
 * its bemesh_messages but messages are treated like classes
 * as they should
 */

#include "bemesh_messages_v2.hpp"
#include <map>

namespace bemesh {
  MessageHeader::MessageHeader():
    m_dest_addr(),
    m_src_addr(),
    m_id(),
    m_hops(),
    m_seq(),
    m_psize(){}
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
  
  uint8_t MessageHeader::size(void) const{
    return MESSAGE_HEADER_DATA_SIZE+m_psize;
  }
  IndexedMessage::IndexedMessage():
    MessageHeader(),m_entries() {}
  // Notice the t_psize+1 payload size for indexedMessages (+1 accounts for m_entries val in the payload)
  IndexedMessage::IndexedMessage(std::size_t t_entries, dev_addr_t t_dest, dev_addr_t t_src, uint8_t t_id,
				 uint8_t t_hops, uint8_t t_seq, uint8_t t_psize):
    MessageHeader(t_dest, t_src, t_id, t_hops, t_seq, t_psize+1), m_entries(t_entries) {}
  
  std::size_t IndexedMessage::entries(void) {
    return m_entries;
  }

  RoutingDiscoveryRequest::RoutingDiscoveryRequest():
    MessageHeader() {}
  RoutingDiscoveryRequest::RoutingDiscoveryRequest(dev_addr_t t_dest, dev_addr_t t_src):
    MessageHeader(t_dest, t_src, ROUTING_DISCOVERY_REQ_ID, 0, 0, 0){}

  RoutingDiscoveryResponse::RoutingDiscoveryResponse():
    IndexedMessage(), m_payload() {}
  
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

  RoutingUpdateMessage::RoutingUpdateMessage():
    IndexedMessage(), m_payload() {}
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

  void MessageHeader::serialize(std::ostream& out) const {
    // Place ID and Size first
    out.write(reinterpret_cast<const char*>(&m_id), sizeof(m_id));
    // Place dest and src addresses
    out.write(reinterpret_cast<const char*>(&m_dest_addr), sizeof(dev_addr_t));
    out.write(reinterpret_cast<const char*>(&m_src_addr), sizeof(dev_addr_t));
    // Replace the id
    out.write(reinterpret_cast<const char*>(&m_id), sizeof(m_id));
    // finally place hops, seq and psize
    out.write(reinterpret_cast<const char*>(&m_hops), sizeof(m_hops));
    out.write(reinterpret_cast<const char*>(&m_seq), sizeof(m_seq));
    out.write(reinterpret_cast<const char*>(&m_psize), sizeof(m_psize));
    return;
  }

  void IndexedMessage::serialize(std::ostream&out) const {
    MessageHeader::serialize(out);
    out.write(reinterpret_cast<const char*>(&m_entries), sizeof(m_entries));
    return;
  }
  

  void RoutingDiscoveryRequest::serialize(std::ostream&out) const {
    MessageHeader::serialize(out);
    return;
  }
  
  void RoutingDiscoveryResponse::serialize(std::ostream&out) const {
    IndexedMessage::serialize(out);
    // stream out the payload
    for(int i=0;i<m_entries;++i) {
      out.write(reinterpret_cast<const char*>(&m_payload[i]), sizeof(routing_params_t));
    }
  }
  
  void RoutingUpdateMessage::serialize(std::ostream&out) const {
    IndexedMessage::serialize(out);
    for(int i=0;i<m_entries;++i) {
      out.write(reinterpret_cast<const char*>(&m_payload[i]), sizeof(routing_update_t));
    }
  }

  //typedef MessageHeader* (*msg_ctor_t)(std::istream&);
  static std::map<uint8_t, MessageHeader*> _serial_ctor_map =
    {
      {ROUTING_DISCOVERY_REQ_ID, new RoutingDiscoveryRequest()},
      {ROUTING_DISCOVERY_RES_ID, new RoutingDiscoveryResponse()},
      {ROUTING_UPDATE_ID, new RoutingUpdateMessage()},
    };
  
  MessageHeader* MessageHeader::unserialize(std::istream& istr) {
    uint8_t incoming_id;
    istr.read(reinterpret_cast<char*>(&incoming_id), 1);
    return _serial_ctor_map[incoming_id]->create(istr);
  }

  MessageHeader* MessageHeader::create(std::istream&) {return nullptr;}
  
  RoutingDiscoveryRequest* RoutingDiscoveryRequest::create(std::istream& istr) {
    // Read header
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_id), sizeof(m_id));
    istr.read(reinterpret_cast<char*>(&m_hops), sizeof(m_hops));
    istr.read(reinterpret_cast<char*>(&m_seq), sizeof(m_seq));
    istr.read(reinterpret_cast<char*>(&m_psize), sizeof(m_psize));    
    return this;
  }
  
  RoutingDiscoveryResponse* RoutingDiscoveryResponse::create(std::istream& istr) {
    // Read header
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_id), sizeof(m_id));
    istr.read(reinterpret_cast<char*>(&m_hops), sizeof(m_hops));
    istr.read(reinterpret_cast<char*>(&m_seq), sizeof(m_seq));
    istr.read(reinterpret_cast<char*>(&m_psize), sizeof(m_psize));
    // Read the payload
    istr.read(reinterpret_cast<char*>(&m_entries), sizeof(m_entries));
    for(int i=0;i<m_entries;++i) {
      routing_params_t temp_entry;
      istr.read(reinterpret_cast<char*>(&temp_entry), sizeof(routing_params_t));
      m_payload[i]=temp_entry;
    }
    return this;
  }
  
  RoutingUpdateMessage* RoutingUpdateMessage::create(std::istream& istr) {
    return nullptr;
  }
    
}
