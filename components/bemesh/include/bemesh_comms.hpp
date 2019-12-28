/*
 * bemesh_comms.hpp
 * Library for higher abstraction on message passing between ESP32 nodes
 * The CommHandler object contains all the buffers for the messages that
 * needs to be parsed.
 */

#pragma once
#include "bemesh_messages.hpp"
#include "routing.hpp"

#include <array>

namespace bemesh {

  
  struct CommHandler {
    std::array<MessageHeader*, MESSAGE_TYPES_MAX> m_msg_buffers;
    CommHandler(dev_addr_t t_node_addr, std::size_t t_buffer_size);
    //template<typename T>
    //ErrStatus installOps(uint8_t t_id, );
  };
}
