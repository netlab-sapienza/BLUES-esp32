/*
 * message_handler.cpp
 */

#include "message_handler.h"

namespace bemesh {

  //Message parsing callbacks
  // dest/src addresses
  static ErrStatus pars_addr_cb(MessageHandler* t_mh, uint8_t c);
  // message_id 
  static ErrStatus pars_id_cb(MessageHandler* t_mh, uint8_t c);
  // hops, seq
  static ErrStatus pars_hinfo_cb(MessageHandler* t_mh, uint8_t c);
  // payload size
  static ErrStatus pars_payload_cb(MessageHandler* t_mh, uint8_t c);
  // data
  static ErrStatus pars_payload_cb(MessageHandler* t_mh, uint8_t c);

    

  MessageHandler::MessageHandler(void): ops_vect() {
    // empty the buffer
    tx_size=0;
    // initiate the parser FSM
    pars_cb=pars_addr_cb;
  }

  ErrStatus MessageHandler::rxByte(uint8_t c) {
    ErrStatus ret;
    ret=(*pars_cb)(this, c);
    if(ret==ParsingComplete) {
      // execute the correct callback
      (*tx_op->recv_cb)(tx_op->recv_cb_args);
    }
    return ret;
  }












  // dest/src addresses
  static ErrStatus pars_addr_cb(MessageHandler* t_mh, uint8_t c) {
    if(t_mh->tx_size < 12) {
      // Handle only initial 12 bytes
      uint16_t *tx_idx=&t_mh->tx_size;
      t_mh->tx_buffer[*tx_idx]=c;
      ++(*tx_idx);
      // if the cb got called 12 times (12 bytes)
      // pass to the next cb
      if(*tx_idx == 12) {
	t_mh->pars_cb=pars_id_cb;
      }
      return Success;
    } else {
      // if something goes wrong, reset the fsm
      t_mh->tx_size=0;
      t_mh->pars_cb=pars_addr_cb;
      return GenericError;
    }    
  }
  // message_id 
  static ErrStatus pars_id_cb(MessageHandler* t_mh, uint8_t c) {
    // If invalid byte arrives, stop the fsm
    if(c > MESSAGE_TYPES_MAX) {
      t_mh->tx_size=0;
      t_mh->pars_cb=0;
      t_mh->pars_cb=pars_addr_cb;
    }
    return Success;
  }
  // hops, seq
  static ErrStatus pars_hinfo_cb(MessageHandler* t_mh, uint8_t c);
  // payload size
  static ErrStatus pars_payload_cb(MessageHandler* t_mh, uint8_t c);
  // data
  static ErrStatus pars_payload_cb(MessageHandler* t_mh, uint8_t c);
  
}
