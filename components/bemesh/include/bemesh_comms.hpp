/*
 * bemesh_comms.hpp
 * Library for higher abstraction on message passing between ESP32 nodes
 * The CommHandler object contains all the buffers for the messages that
 * needs to be parsed.
 */

#pragma once
#include "bemesh_messages.hpp"
#include "message_handler.hpp"
#include "routing.hpp"
#include "bemesh_utils.hpp"
#include <memory>
#include <array>

namespace bemesh {
  enum MessageOps {
    Copy=0x01,
    // TODO
  };
  typedef void (*msg_event_cb)(void* args);

  
  struct CommHandler {
    std::array<message_ops_t*, MESSAGE_TYPES_MAX> m_ops;
    ring_buffer<MessageHeader*> m_incoming_messages;
    MessageHandler m_mhandler;
    
    explicit CommHandler(std::size_t t_buffer_size);
    ErrStatus addOps(uint8_t t_id, uint8_t t_max_size, uint8_t t_flags,
		     msg_event_cb t_cb, void* args);

    ErrStatus removeOps(uint8_t t_id);

    ErrStatus sendMessage(MessageHeader* const t_header);

    bool empty(void);
    MessageHeader* readMessage(void);

    void handle(void);
  };
}
