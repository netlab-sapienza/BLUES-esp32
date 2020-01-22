/*
 * bemesh_comms.h
 * Library for higher abstraction on message passing between ESP32 nodes
 * The CommHandler object contains all the buffers for the messages that
 * needs to be parsed.
 */

#include "bemesh_comms.hpp"
#include "bemesh_utils.hpp"

namespace bemesh {
  enum MessageOps {
    Copy=0x01,
    Relay=0x02,
  };
  
  struct message_args_t {
    
  };
  
  CommHandler::CommHandler(dev_addr_t t_node_addr, std::size_t t_buffer_size):m_msg_buffers() {
    
  }


  
}
