/**
 * message_handler_v3.hpp
 * A synchronous implementation of message_handler_v2.
 * Copyright [2020] <Emanuele Giacomini>
 */

#include "message_handler_v3.hpp"
#include <sstream>
extern "C" {
#include "esp_log.h"
}

static const char *TAG="message_handler";

namespace bemesh {
  // Private constructor.
  MessageHandler::MessageHandler(void) {}
  
  /**
   * Returns the reference to the only instance of MessageHandler present in 
   * the program.
   * @return reference to m_inst.
   */
  MessageHandler &MessageHandler::getInstance(void) {
    static MessageHandler m_inst;
    return m_inst;
  }

  /**
   * Prepare a message referenced through its header h to be sent.
   * Serialize will fill buf_ptr and buf_len with the correct values
   * which can be used to access the transmission buffer later.
   *
   * @param h Pointer to the message's header that has to be sent.
   * @param buf_ptr pointer to the serialized message buffer
   * @param buf_len pointer to the serialized message buffer length value
   * @return Success if no errors occurred, !=Success otherwhise.
   */
  ErrStatus MessageHandler::serialize(MessageHeader *h, uint8_t **buf_ptr, uint16_t *buf_len) {
    std::stringstream serialized_stream;
    // clean the stream.
    std::stringstream().swap(serialized_stream);
    // reset the buffer length
    m_tx_buf_len = 0;
    // Serialize the message h into serialized_stream
    h->serialize(serialized_stream);
    // Transfer the serialized payload into the internal transmission buffer.
    // m_tx_buf;
    // m_tx_buf_len
    std::size_t payload_dim=h->psize()+MESSAGE_HEADER_DATA_SIZE;
    /* The payload dim should also consider:
     * sizeof(std::size_t) bytes from payload_dim
     * sizeof(uint8_t) bytes from the copy of m->id() during serialization.
     */
    payload_dim += sizeof(std::size_t) + sizeof(uint8_t);
    if (payload_dim > MESSAGE_HANDLER_TX_BUF_SIZE) {
      // TODO(Emanuele): empty the serialized stream.
      *buf_ptr = NULL;
      *buf_len = 0;
      return BufferFullError;
    }
    // Set the buffer len as payload dim.
    *buf_len = payload_dim;
    // Load operation will be execute in the following manner
    // Load the total_buffer_dimension
    // Load the serialized message data from the serialized_stream
    // keep in mind that serialized_stream starts with the ID of the message
    // followed by the ordered serialization data.
    ((std::size_t *)m_tx_buf)[0] = payload_dim;
    m_tx_buf_len += sizeof(std::size_t);
    payload_dim -= sizeof(std::size_t);
    for(int i=0; i < payload_dim ; ++i) {
      serialized_stream.read(reinterpret_cast<char*>(&m_tx_buf[m_tx_buf_len++]),
			     sizeof(uint8_t));
    }
    // Setup the buffer pointer
    *buf_ptr = (uint8_t *)&m_tx_buf;
    ESP_LOGI(TAG, "built payload:");
    ESP_LOG_BUFFER_HEX(TAG, *buf_ptr, *buf_len);
    return Success;
  }

  /**
   * Reads a buffer src of length len and tries to parse a message from it.
   * the src buffer format should be of the form:
   *  std::size_t buffer_length
   *  uint8_t message_id
   *  uint8_t message[message_len]
   * Where message_len is 
   *  (buffer_length-sizeof(std::size_t)-sizeof(uint8))
   * PLEASE TAKE CARE: The returned message is heap-based therefore,
   * it should be deallocated accordingly after its usage.
   *
   * @param src buffer of bytes containing a serialized message
   * @param len src buffer length
   * @return 
   */
  MessageHeader *MessageHandler::unserialize(uint8_t *src, uint16_t len) {
    // TODO(Emanuele): Use len to cross-check the buffer_length value.
    ESP_LOGI(TAG, "unserialized called.");
    std::stringstream dstream;
    std::size_t buffer_length = (std::size_t)src[0];
    buffer_length -= sizeof(std::size_t);
    src += sizeof(std::size_t);
    ESP_LOGI(TAG, "need to read %d bytes.", buffer_length);
    uint8_t message_id = src[0];
    if (message_id >= MESSAGE_TYPE_MAX) {
      // Check for invalid messages before parsing them.
      return NULL;
    }
    // Fill the deserialize stream with the message bytes.
    for (int i = 0; i < buffer_length; ++i) {
      dstream.write(reinterpret_cast<char*>(src+i),
		    sizeof(uint8_t));
    }

    // Launch the deserialization process.
    MessageHeader* new_message = MessageHeader::unserialize(dstream);
    if(!new_message) {
      return NULL;
    } else {
      return new_message;
    }
  }
}
