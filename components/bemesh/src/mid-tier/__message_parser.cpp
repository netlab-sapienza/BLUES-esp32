/*
 * __message_parser.cpp
 * Handles message parsing
 */

#include "__message_parser.hpp"

namespace bemesh {  
  MessageParser::MessageParser(){
    for(uint8_t i=0; i<MSG_PARSER_BUF_SIZE; ++i) {
      m_in_buf[i] = 0;
    }
    m_buf_idx=0;

    m_on_recv_fn=nullptr;
    m_on_recv_args=nullptr;
  }

  ErrStatus MessageParser::reset(void) {    
    m_buf_idx=0;
    return Success;
  }

  ErrStatus MessageParser::feed(uint8_t t_char) {
    if(m_buf_idx==MSG_PARSER_BUF_SIZE) {
      return BufferFullError;
    }
    m_in_buf[m_buf_idx++] = t_char;
    // advance the parsing machine
    ErrStatus ret;
    ret = (*m_curr_parse_fn)(t_char);
    // If the parsing machine had an error
    // return the error
    if(ret<0) {
      return ret;
    }
    // If parsing is complete, try to execute the
    // receive callback
    if(ret == ParsingComplete){
      if (m_on_recv_fn == nullptr){
	return NoCallbackError;
      }
      (*m_on_recv_fn)(m_on_recv_args);
      reset();
      return ret;
    }
    return Success;
  }  
}
