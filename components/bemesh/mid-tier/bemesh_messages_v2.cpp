/*
 * bemesh_messages_v2.cpp
 * its bemesh_messages but messages are treated like classes
 * as they should
 */

#include "bemesh_messages_v2.hpp"
#include <map>
#include <algorithm>

extern "C" {
  #include "esp_log.h"
}

static const char *TAG="messages";

namespace bemesh {


  dev_addr_t const BROADCAST_ADDR = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  
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

  MessageHeader::MessageHeader(const MessageHeader& h) {
    m_dest_addr=h.m_dest_addr;
    m_src_addr=h.m_src_addr;
    m_id=h.m_id;
    m_hops=h.m_hops;
    m_seq=h.m_seq;
    m_psize=h.m_psize;
  }

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

  
  void MessageHeader::setBroadcast(void) {
    m_dest_addr=(dev_addr_t)BROADCAST_ADDR;
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
  
  RoutingDiscoveryResponse::RoutingDiscoveryResponse(dev_addr_t t_dest, dev_addr_t t_src,
			   std::vector<routing_params_t> &t_payload,
			   std::size_t t_pentries):
    IndexedMessage(t_pentries, t_dest, t_src, ROUTING_DISCOVERY_RES_ID, 0, 0, 0) {
    // Copy the payload from t_payload (vector) into m_payload (array)
    std::copy_n(t_payload.begin(), t_pentries, m_payload.begin());
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

  RoutingUpdateMessage::RoutingUpdateMessage(dev_addr_t t_dest, dev_addr_t t_src,
					     std::vector<routing_update_t> t_payload,
					     std::size_t t_pentries):
    IndexedMessage(t_pentries, t_dest, t_src, ROUTING_UPDATE_ID, 0, 0, 0){
    // Copy the payload from t_payload (vector) into m_payload (array)
    std::copy_n(t_payload.begin(), t_pentries, m_payload.begin());
    // Update the payload size based on t_payload used size
    m_psize+=sizeof(routing_params_t)*t_pentries;
  }
  

  std::array<routing_update_t, ROUTING_UPDATE_ENTRIES_MAX> RoutingUpdateMessage::payload(void) {
    return m_payload;
  }

  RoutingSyncMessage::RoutingSyncMessage():IndexedMessage(), m_payload() {}
  RoutingSyncMessage::RoutingSyncMessage(dev_addr_t t_dest, dev_addr_t t_src,
					 std::array<uint8_t,
					 ROUTING_SYNC_ENTRIES_MAX> t_payload,
					 std::size_t t_pentries):
    IndexedMessage(t_pentries, t_dest, t_src, ROUTING_SYNC_ID, 0, 0, 0), m_payload(t_payload) {
    m_psize+=sizeof(uint8_t)*t_pentries;
  }

  std::array<uint8_t, ROUTING_SYNC_ENTRIES_MAX> RoutingSyncMessage::payload(void) {
    return m_payload;
  }
  
  RoutingPingMessage::RoutingPingMessage() : MessageHeader(), m_pong_flag(0) {}
  RoutingPingMessage::RoutingPingMessage(dev_addr_t t_dest, dev_addr_t t_src, uint8_t t_pong):
    MessageHeader(t_dest, t_src, ROUTING_PING_ID, 0, 0, 0), m_pong_flag(t_pong){
    m_psize+=sizeof(m_pong_flag);
  }

  uint8_t RoutingPingMessage::pong_flag(void) const {
    return m_pong_flag;
  }

  

  void MessageHeader::serialize(std::ostream& out) const {
    // Place ID first
    out.write(reinterpret_cast<const char*>(&m_id), sizeof(m_id));
    // place the parameters of the header
    out.write(reinterpret_cast<const char*>(&m_dest_addr), sizeof(dev_addr_t));
    out.write(reinterpret_cast<const char*>(&m_src_addr), sizeof(dev_addr_t));
    out.write(reinterpret_cast<const char*>(&m_id), sizeof(m_id));
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

  void RoutingSyncMessage::serialize(std::ostream&out) const {
    IndexedMessage::serialize(out);
    for(int i=0;i<m_entries;++i) {
      out.write(reinterpret_cast<const char*>(&m_payload[i]), sizeof(uint8_t));
    }
  }

  void RoutingPingMessage::serialize(std::ostream&out) const {
    MessageHeader::serialize(out);
    out.write(reinterpret_cast<const char*>(&m_pong_flag), sizeof(m_pong_flag));
  }
  
  //typedef MessageHeader* (*msg_ctor_t)(std::istream&);
  static std::map<uint8_t, MessageHeader*> _serial_ctor_map =
    {
      {ROUTING_DISCOVERY_REQ_ID, new RoutingDiscoveryRequest()},
      {ROUTING_DISCOVERY_RES_ID, new RoutingDiscoveryResponse()},
      {ROUTING_UPDATE_ID, new RoutingUpdateMessage()},
      {ROUTING_SYNC_ID, new RoutingSyncMessage()},
      {ROUTING_PING_ID, new RoutingPingMessage()},
    };
  
  MessageHeader* MessageHeader::unserialize(std::istream& istr) {
    uint8_t incoming_id;
    istr.read(reinterpret_cast<char*>(&incoming_id), 1);
    ESP_LOGI(TAG, "received id no. %02x\n", incoming_id);
    
    return _serial_ctor_map[incoming_id]->create(istr);
  }

  MessageHeader* MessageHeader::create(std::istream&) {return nullptr;}
  
  RoutingDiscoveryRequest* RoutingDiscoveryRequest::create(std::istream& istr) {
    // Read header
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_src_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_id), sizeof(m_id));
    istr.read(reinterpret_cast<char*>(&m_hops), sizeof(m_hops));
    istr.read(reinterpret_cast<char*>(&m_seq), sizeof(m_seq));
    istr.read(reinterpret_cast<char*>(&m_psize), sizeof(m_psize));    
    return this;
  }
  
  RoutingDiscoveryResponse* RoutingDiscoveryResponse::create(std::istream& istr) {
    // Read header
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_src_addr), sizeof(dev_addr_t));
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
      routing_update_t temp_entry;
      istr.read(reinterpret_cast<char*>(&temp_entry), sizeof(routing_update_t));
      m_payload[i]=temp_entry;
    }
    return this;
  }

  RoutingSyncMessage* RoutingSyncMessage::create(std::istream& istr) {
    // Read header
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_src_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_id), sizeof(m_id));
    istr.read(reinterpret_cast<char*>(&m_hops), sizeof(m_hops));
    istr.read(reinterpret_cast<char*>(&m_seq), sizeof(m_seq));
    istr.read(reinterpret_cast<char*>(&m_psize), sizeof(m_psize));
    // Read the payload
    istr.read(reinterpret_cast<char*>(&m_entries), sizeof(m_entries));
    for(int i=0;i<m_entries;++i) {
      uint8_t temp_entry;
      istr.read(reinterpret_cast<char*>(&temp_entry), sizeof(uint8_t));
      m_payload[i]=temp_entry;
    }
    return this;
  }

  RoutingPingMessage* RoutingPingMessage::create(std::istream& istr) {
    // Read header
    istr.read(reinterpret_cast<char*>(&m_dest_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_src_addr), sizeof(dev_addr_t));
    istr.read(reinterpret_cast<char*>(&m_id), sizeof(m_id));
    istr.read(reinterpret_cast<char*>(&m_hops), sizeof(m_hops));
    istr.read(reinterpret_cast<char*>(&m_seq), sizeof(m_seq));
    istr.read(reinterpret_cast<char*>(&m_psize), sizeof(m_psize));
    // Read payload
    istr.read(reinterpret_cast<char*>(&m_pong_flag), sizeof(uint8_t));
    return this;
  }
    
}
