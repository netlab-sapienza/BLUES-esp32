/*
 * message_handler_v2.hpp
 */

#pragma once
#include "bemesh_messages_v2.hpp"
#include "bemesh_status.hpp"
#include <sstream>

namespace bemesh {

  typedef void(*MsgSendCb)(uint8_t* t_buf, uint8_t t_len, MessageHeader* t_header, void* args);
  typedef void (*MsgRecvCb)(MessageHeader* t_header, void* args);
  struct message_ops_t {
    MsgRecvCb recv_cb;
    void* args;
    message_ops_t(MsgRecvCb t_cb, void* t_args):recv_cb(t_cb),args(t_args) {};
  };

  struct message_tx_ops_t {
    MessageHeader header_stub;
    void* args;
    message_tx_ops_t(void):
      header_stub(), args() {}
    message_tx_ops_t(MessageHeader* t_h, void* t_args);
  };
  
  class MessageHandler {
    std::array<message_ops_t*, MESSAGE_TYPE_MAX> m_ops;
    std::stringstream m_rx_strm;
    std::stringstream m_tx_strm;
    std::size_t m_rx_entries;
    std::size_t m_tx_entries;

    std::array<message_ops_t*, MESSAGE_TYPE_MAX> m_tx_ops;
    MsgSendCb m_send_cb;
    uint8_t* m_tx_buf;
  public:
    MessageHandler(void):m_ops(),m_rx_strm(),m_tx_strm(),
			 m_rx_entries(0), m_tx_entries(0),
			 m_tx_ops(),m_send_cb(),
			 m_tx_buf(){};
    
    ErrStatus installOps(uint8_t t_id, MsgRecvCb t_cb, void* t_args);
    ErrStatus installTxOps(uint8_t t_id, void* t_args);
    ErrStatus installTxCb(MsgSendCb t_cb);
    ErrStatus installTxBuffer(uint8_t* t_buf);

    ErrStatus send(MessageHeader* t_h);
    std::size_t read(uint8_t* t_src);

    void handle(void);
  };
}
