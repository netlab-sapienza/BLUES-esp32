/*
 * message_handler.h
 */

#pragma once
#include "bemesh_messages.hpp"
#include "__status.hpp"

#include <array>

namespace bemesh {

  struct MessageHandler;
  
  typedef ErrStatus (*MsgParsCb)(MessageHandler* t_mh, uint8_t c);
  typedef void (*MsgRecvCb)(void* args);

  struct message_ops_t {
    uint8_t message_id;
    MsgRecvCb recv_cb;
    void* recv_cb_args;
  };

#define MH_TX_BUF_SIZE 256

  struct MessageHandler {
    // Parse vars
    uint8_t tx_buffer[MH_TX_BUF_SIZE];
    uint16_t tx_size;
    MsgParsCb pars_cb;
    message_ops_t* tx_op;

    std::array<message_ops_t*, MESSAGE_TYPES_MAX> ops_vect;

    
    MessageHandler(void);
    void _set_pars_cb(MsgParsCb t_cb);
    ErrStatus rxByte(uint8_t c);

    ErrStatus installOps(message_ops_t t_ops);
  };
}
