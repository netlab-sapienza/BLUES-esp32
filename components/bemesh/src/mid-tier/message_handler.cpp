/*
 * message_handler.cpp
 */

#include "message_handler.hpp"
#include <cstring>
#include <cassert>

namespace bemesh {

  //Message parsing callbacks
  // dest/src addresses
  static ErrStatus pars_addr_cb(MessageHandler* t_mh, uint8_t c);
  // message_id 
  static ErrStatus pars_id_cb(MessageHandler* t_mh, uint8_t c);
  // hops, seq
  static ErrStatus pars_hinfo_cb(MessageHandler* t_mh, uint8_t c);
  // payload size
  static ErrStatus pars_size_cb(MessageHandler* t_mh, uint8_t c);
  // data
  static ErrStatus pars_payload_cb(MessageHandler* t_mh, uint8_t c);

    

  MessageHandler::MessageHandler(void): ops_vect() {
    // empty the buffer
    rx_size=0;
    // initiate the parser FSM
    pars_cb=pars_addr_cb;
  }

  ErrStatus MessageHandler::rxByte(uint8_t c) {
    return (*pars_cb)(this, c);
  }

  ErrStatus MessageHandler::installOps(message_ops_t* t_ops) {
    uint8_t ops_id=t_ops->message_id;
    if(ops_vect[ops_id]!=nullptr) {
      // An operation was already installed
      return GenericError;
    } else {
      ops_vect[ops_id]=t_ops;
      return Success;
    }
  }
  
  ErrStatus MessageHandler::prepareMessage(MessageHeader* t_header) {
    // Must copy the whole message inside the tx_buffer
    // Copy the header
    tx_size=0;
    std::copy((uint8_t*)t_header, (uint8_t*)(t_header+sizeof(MessageHeader)), tx_buffer);
    tx_size+=sizeof(MessageHeader);
    std::size_t payload_size=t_header->payload_size;
    // Iterator for payload copy
    uint8_t* it=(uint8_t*)t_header;
    uint8_t* dest_it=(uint8_t*)tx_buffer;
    // shift forward by sizeof(MessageHeader)
    it+=sizeof(MessageHeader);
    dest_it+=sizeof(MessageHeader);
    for(int i=0;i<payload_size;++i) {
      *dest_it=*it;
      ++it;
      ++dest_it;
      tx_size+=1;
    }
    return Success;
  }
  















  

  /*
   * FSM (Finite State Machine) For message parsing
   * Here are the callbacks for each of its states
   */

  // dest/src addresses
  static ErrStatus pars_addr_cb(MessageHandler* t_mh, uint8_t c) {
    if(t_mh->rx_size < 12) {
      // Handle only initial 12 bytes
      uint8_t *rx_idx=&t_mh->rx_size;
      t_mh->rx_buffer[*rx_idx]=c;
      ++(*rx_idx);
      // if the cb got called 12 times (12 bytes)
      // pass to the next cb
      if(*rx_idx == 12) {
	t_mh->pars_cb=pars_id_cb;
      }
      return Success;
    } else {
      // if something goes wrong, reset the fsm
      t_mh->rx_size=0;
      t_mh->pars_cb=pars_addr_cb;
      return GenericError;
    }    
  }
  // message_id 
  static ErrStatus pars_id_cb(MessageHandler* t_mh, uint8_t c) {
    // If invalid byte arrives, stop the fsm
    if(c > MESSAGE_TYPES_MAX) {
      t_mh->rx_size=0;
      t_mh->pars_cb=pars_addr_cb;
      return GenericError;
    } else {
      // if id is valid, check that current op
      // is supported
      t_mh->rx_op=t_mh->ops_vect[c];
      if(t_mh->rx_op==nullptr) {
	// not supported, reset the fsm
	t_mh->rx_size=0;
	t_mh->pars_cb=pars_addr_cb;
	return NoCallbackError;
      }
      t_mh->rx_buffer[t_mh->rx_size]=c;
      t_mh->pars_cb=pars_hinfo_cb;
      t_mh->rx_size+=1;
      return Success;
    }
  }
  // hops, seq
  static ErrStatus pars_hinfo_cb(MessageHandler* t_mh, uint8_t c) {
    // need to read byte 14 and 15
    t_mh->rx_buffer[t_mh->rx_size]=c;
    t_mh->rx_size+=1;
    if(t_mh->rx_size==15) {
      t_mh->pars_cb=pars_size_cb;
    }
    return Success;
  }
  // payload size
  static ErrStatus pars_size_cb(MessageHandler* t_mh, uint8_t c) {
    // check that current size is less or equal than expected size
    if(c > t_mh->rx_op->max_payload_size) {
      t_mh->rx_size=0;
      t_mh->pars_cb=pars_addr_cb;
      t_mh->rx_op=nullptr;
      return WrongPayloadSize;
    }
    // update the final size
    t_mh->rx_end_size=t_mh->rx_size+c;
    // insert byte
    t_mh->rx_buffer[t_mh->rx_size++]=c;
    t_mh->pars_cb=pars_payload_cb;
    // Notice that some packets may not bring any payload
    // In order to handle such messages, a check with the sizes
    // is necessary.
    //printf("recv_size:%d\tmax_size:%d\n", c, t_mh->rx_op->max_payload_size);
    if(!c&&!t_mh->rx_op->max_payload_size) {
      // If we do not expect any payload,
      // fast forward 
      return (*pars_payload_cb)(t_mh, c);
    }
    return Success;
  }
  // data
  static ErrStatus pars_payload_cb(MessageHandler* t_mh, uint8_t c) {
    //printf("current:%d\tfinal:%d\n", t_mh->rx_size, t_mh->rx_end_size);
    if(t_mh->rx_size==t_mh->rx_end_size) {
      // payload parsing is complete
      // copy the message if the rx_op
      // requires it.
      message_ops_t* rx_op=t_mh->rx_op;
      if(rx_op->copy_ptr!=nullptr) {
	// copy size must be greater(or eq) than message size
	assert(t_mh->rx_end_size <= rx_op->copy_size);
	memcpy(rx_op->copy_ptr, t_mh->rx_buffer, t_mh->rx_end_size);
      }
      // execute the receive cb if rx_op requires it
      if(rx_op->recv_cb!=nullptr) {
	(*rx_op->recv_cb)((MessageHeader*)t_mh->rx_buffer, rx_op->recv_cb_args);
      }
      // reset the fsm
      t_mh->rx_size=0;
      t_mh->rx_end_size=0;
      t_mh->pars_cb=pars_addr_cb;
      t_mh->rx_op=nullptr;
      return ParsingComplete;
    } else {
      // payload parsing in action
      t_mh->rx_buffer[t_mh->rx_size++]=c;
      return Success;
    }
  }
  
}
