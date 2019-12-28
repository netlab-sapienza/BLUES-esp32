/*
 * bemesh_comms.h
 * Library for higher abstraction on message passing between ESP32 nodes
 * The CommHandler object contains all the buffers for the messages that
 * needs to be parsed.
 */

#include "bemesh_comms.hpp"
#include <cassert>

namespace bemesh {

  static void message_handling_routine_cb(MessageHeader* t_header,
					  void* t_args);
  struct message_args_t {
    uint8_t op_flags;
    msg_event_cb event_cb;
    void* event_cb_args;
    ring_buffer<MessageHeader*> *buf_ptr;
    message_args_t(uint8_t t_flags, msg_event_cb t_cb, void* t_args,
		   ring_buffer<MessageHeader*> *t_buf):op_flags(t_flags),event_cb(t_cb),event_cb_args(t_args),buf_ptr(t_buf) {}
  };
  
  CommHandler::CommHandler(std::size_t t_buffer_size):m_ops(),m_incoming_messages(t_buffer_size),
						      m_mhandler(){}

  ErrStatus CommHandler::addOps(uint8_t t_id, uint8_t t_max_size, uint8_t t_flags,
				msg_event_cb t_cb, void* t_args) {
    if(m_ops[t_id]!=nullptr) {
      return BufferFullError;
    }
    m_ops[t_id] = new message_ops_t;
    message_ops_t* op=m_ops[t_id];
    op->message_id=t_id;
    op->max_payload_size=t_max_size;
    op->recv_cb=&message_handling_routine_cb;
    op->recv_cb_args=new message_args_t(t_flags, t_cb, t_args, &m_incoming_messages);
    op->copy_ptr=nullptr;
    op->copy_size=0;
    if(m_mhandler.installOps(op)!=Success) {
      delete m_ops[t_id];
    }    
    return Success;
  }

  ErrStatus CommHandler::removeOps(uint8_t t_id) {
    if(m_ops[t_id]==nullptr) {
      return GenericError;
    }
    delete m_ops[t_id];
    m_ops[t_id]=nullptr;
    return Success;
  }

  ErrStatus CommHandler::sendMessage(MessageHeader* const t_header) {
    // TODO
    return Success;
  }
  
  bool CommHandler::empty(void) {
    return m_incoming_messages.empty();
  }
  
  MessageHeader* CommHandler::readMessage(void) {
    return m_incoming_messages.get();
  }

  void CommHandler::handle(void) {
    // Handle should parse 
    return;
  }


  static void message_handling_routine_cb(MessageHeader* t_header,
					  void* t_args) {
    
    // Handle every type of message
    message_args_t* args=reinterpret_cast<message_args_t*>(t_args);
    uint8_t operations=args->op_flags;
    ring_buffer<MessageHeader*> *buf_ptr=args->buf_ptr;
    

    // Copy the message in the ring buffer
    // Allocate the space for the vector
    uint8_t* _entry(new uint8_t[MESSAGE_SIZE_MAX]);
    // dump the input message in the vector
    std::copy((uint8_t*)t_header, ((uint8_t*)t_header)+MESSAGE_SIZE(t_header),
	      _entry);
    // place the vector pointer to the circular buffer
    buf_ptr->put((MessageHeader*)_entry);
    // TODO: Add operations
    if(args->event_cb!=nullptr) {
      // if possible, execute the callback
      (*args->event_cb)(args->event_cb_args);
    }
    return;
  }
  
}
