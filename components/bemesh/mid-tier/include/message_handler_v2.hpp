/*
 * message_handler_v2.hpp
 */

#pragma once
#include "bemesh_messages_v2.hpp"
#include "bemesh_status.hpp"
#include <sstream>

namespace bemesh {

  // MsgSendCb definition. This function should handle the linkage between the message parser
  // layer and the GATT layer. It's arguments contains the data buffer and its size,
  // a pointer to a cloned header of the sending message, and a customizable void pointer
  typedef void(*MsgSendCb)(uint8_t* t_buf, uint8_t t_len, MessageHeader* t_header, void* args);

  // MsgSendCb definition. This function is linked with a given message id.
  // The function is called when the MessageHandler::deserialize function is executed
  // and a valid message is deserialized.
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
    uint8_t* m_tx_buf_iterator;
  public:
    // Constructor
    MessageHandler(void):m_ops(),m_rx_strm(),m_tx_strm(),
			 m_rx_entries(0), m_tx_entries(0),
			 m_tx_ops(),m_send_cb(),
			 m_tx_buf(){m_tx_buf_iterator=m_tx_buf;}

    // Install the reception callback for t_id messages
    ErrStatus installOps(uint8_t t_id, MsgRecvCb t_cb, void* t_args);
    // Install the transmission callback arguments for t_id messages
    ErrStatus installTxOps(uint8_t t_id, void* t_args);
    // Install the transmission callback. Refer to
    // MsgSendCb definition for its arguments
    ErrStatus installTxCb(MsgSendCb t_cb);
    // Link t_buf buffer for write operations during handle
    ErrStatus installTxBuffer(uint8_t* t_buf);

    // Load the pointed message t_h inside the
    // handler's output stream
    ErrStatus send(MessageHeader* t_h);
    // Load the pointed buffer t_scr inside
    // the handler's input stream
    std::size_t read(uint8_t* t_src);
    // Execute I/O operations for serialization and deserialization.
    // First readings from input stream are executed.
    // Each deserialized message will trigger the relative
    // MsgRecvCb callback inserted in its ops
    // Each serialized message will trigger the
    // MsgSendCb callback with message specific arguments.
    void handle(void);
  };
}
