/**
 * message_handler_v3.hpp
 * A synchronous implementation of message_handler_v2.
 * Copyright [2020] <Emanuele Giacomini>
 */

#include "message_handler_v3.hpp"

namespace bemesh {
  // Private constructor.
  MessageHandler::MessageHandler(void) {}
  
  /**
   * Returns the reference to the only instance of MessageHandler present in 
   * the program.
   * @return reference to m_inst.
   */
  static MessageHandler &MessageHandler::getInstance(void) {
    return m_inst;
  }

  /**
     * Prepare a message referenced through its header h to be sent.
     * After the serialization procedure is complete, the send callback 
     * will be called.
     *
     * @param h Pointer to the message's header that has to be sent.
     * @return Success if no errors occurred, !=Success otherwhise.
     */
    ErrStatus serialize(MessageHeader *h);

    /**
     * Prepare to receive an incoming message through a buffer src
     * of length len.
     */
  ErrStatus unserialize(uint8_t *src, uint16_t len) {

  }

  
}
