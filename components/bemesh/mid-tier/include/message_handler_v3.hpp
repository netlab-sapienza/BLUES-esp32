/**
 * message_handler_v3.hpp
 * A synchronous implementation of message_handler_v2.
 * Copyright [2020] <Emanuele Giacomini>
 */

#pragma once

#include "bemesh_messages_v2.hpp"
#include "bemesh_status.hpp"

#define MESSAGE_HANDLER_TX_BUF_SIZE 256

namespace bemesh {
  /**
   * MessageHandler is a singleton-pattern class that handles 
   * operations related to message serialization and deserialization.
   */
  class MessageHandler {
    static MessageHandler m_inst;
    uint8_t m_tx_buf[MESSAGE_HANDLER_TX_BUF_SIZE];
    uint16_t m_tx_buf_len;
    MessageHandler(void);
  public:
    /**
     * Returns the reference to the only instance of MessageHandler present in 
     * the program.
     * @return reference to m_inst.
     */
    static MessageHandler &getInstance(void);

    /**
     * Prepare a message referenced through its header h to be sent.
     * After the serialization procedure is complete, the send callback 
     * will be called.
     *
     * @param h Pointer to the message's header that has to be sent.
     * @return Success if no errors occurred, !=Success otherwhise.
     */
    MessageHeader &serialize(MessageHeader *h);

    /**
     * Prepare to receive an incoming message through a buffer src
     * of length len.
     */
    ErrStatus unserialize(uint8_t *src, uint16_t len);
  };
}
