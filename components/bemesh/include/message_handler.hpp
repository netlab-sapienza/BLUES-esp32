/*
 * message_handler.h
 */

#pragma once
#include "bemesh_messages.hpp"
#include "bemesh_status.hpp"
#include <array>

namespace bemesh {

  struct MessageHandler;
  
  typedef ErrStatus (*MsgParsCb)(MessageHandler* t_mh, uint8_t c);
  typedef void (*MsgRecvCb)(MessageHeader* t_header, void* args);

  struct message_ops_t {
    // Message ID (REQUIRED)
    uint8_t message_id;
    // maximum payload size
    uint8_t max_payload_size;
    // callback on reception
    MsgRecvCb recv_cb;
    // callback args
    void* recv_cb_args;
    // copy pointer
    MessageHeader* copy_ptr;
    // and its size
    std::size_t copy_size;
  };

#define MH_RX_BUF_SIZE 256
#define MH_TX_BUF_SIZE 256

  struct MessageHandler {
    // Parse vars
    uint8_t rx_buffer[MH_RX_BUF_SIZE];
    uint8_t rx_size;
    uint8_t rx_end_size;
    MsgParsCb pars_cb;
    message_ops_t* rx_op;
    std::array<message_ops_t*, MESSAGE_TYPES_MAX> ops_vect;
    // Message constructor vars
    uint8_t tx_buffer[MH_TX_BUF_SIZE];
    uint8_t tx_size;

    // Constructor
    MessageHandler(void);    

    // Basic primitive for message parsing. The FSM requires one byte per
    // iteration in order to correctly parse it.
    ErrStatus rxByte(uint8_t c);

    // Install a new message operation manager in the message handler.
    // The message_ops_t contains all the valuable informations
    // needed for the parser.
    ErrStatus installOps(message_ops_t* t_ops);

    ErrStatus prepareMessage(MessageHeader* t_header);
  };
}
