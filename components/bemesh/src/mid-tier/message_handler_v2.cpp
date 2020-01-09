/*
 * message_handler_v2.cpp
 */

#include "message_handler_v2.hpp"

namespace bemesh {

  message_tx_ops_t::message_tx_ops_t(MessageHeader* t_h, void* t_args):
    args(t_args) {
    header_stub=*t_h;
  }
  
  ErrStatus MessageHandler::installOps(uint8_t t_id, MsgRecvCb t_cb, void* t_args) {
    if(m_ops[t_id]!=nullptr) {
      return UserInTableError;
    }
    m_ops[t_id]=new message_ops_t(t_cb, t_args);
    return Success;
  }

  ErrStatus MessageHandler::installTxOps(uint8_t t_id, void* t_args) {
    if(m_tx_ops[t_id]!=nullptr) {
      return UserInTableError;
    }
    message_tx_ops_t *tx_args=new message_tx_ops_t;
    tx_args->args=t_args;
    m_tx_ops[t_id]=new message_ops_t(nullptr, (void*)tx_args);
    return Success;
  }

  ErrStatus MessageHandler::installTxCb(MsgSendCb t_cb) {
    if(m_send_cb!=nullptr) {
      return UserInTableError;
    }
    m_send_cb=t_cb;
    return Success;
  }
  
  ErrStatus MessageHandler::installTxBuffer(uint8_t* t_buf) {
    m_tx_buf=t_buf;
    return Success;
  }  
  
  ErrStatus MessageHandler::send(MessageHeader* t_h) {
    // write the message size
    std::size_t msg_len=1+t_h->psize()+MESSAGE_HEADER_DATA_SIZE;
    m_tx_strm.write(reinterpret_cast<char*>(&msg_len), sizeof(std::size_t));
    // write the message
    t_h->serialize(m_tx_strm);
    // copy the header on the right tx_ops packet
    message_ops_t* ops=m_tx_ops[t_h->id()];
    if(ops!=nullptr) {
      message_tx_ops_t* tx_ops=(message_tx_ops_t*)ops->args;
      tx_ops->header_stub=(*t_h);
    }
    m_tx_entries+=1;
    return Success;
  }
  
  std::size_t MessageHandler::read(uint8_t* t_src) {
    // read the message size
    std::size_t msg_len=(std::size_t)*t_src;
    t_src+=sizeof(std::size_t);
    printf("msg size: %d\n", msg_len);
    // insert the whole message in the stream
    for(int i=0;i<msg_len;++i) {
      m_rx_strm.write(reinterpret_cast<char*>(t_src+i), sizeof(uint8_t));
    }
    m_rx_entries+=1;
    return msg_len+sizeof(std::size_t);
  }

  void MessageHandler::handle(void) {
    // reset the tx iterator
    m_tx_buf_iterator=m_tx_buf;
    
    // Execute readings
    while(m_rx_entries>0) {
      --m_rx_entries;
      MessageHeader* recv_msg=MessageHeader::unserialize(m_rx_strm);
      if(recv_msg==nullptr) {
	continue;
      } else {
	uint8_t recv_id=recv_msg->id();
	message_ops_t* ops=m_ops[recv_id];
	if(ops==nullptr) {
	  // remember to implement dtor pls :(
	  //delete recv_msg;
	} else {
	  (*ops->recv_cb)(recv_msg, ops->args);
	}
      }
      
    }
    
    // Execute writings
    while(m_tx_entries>0) {
      uint8_t* tx_it=m_tx_buf_iterator;
      // Read first std::size_t from the stream
      m_tx_strm.read(reinterpret_cast<char*>(tx_it), sizeof(std::size_t));
      // parse the message size;
      std::size_t msg_len=*(std::size_t*)tx_it;
      // forward the buffer
      tx_it+=sizeof(std::size_t);
      uint8_t msg_id=(uint8_t)m_tx_strm.peek();
      for(int i=0;i<msg_len;++i) {
	m_tx_strm.read(reinterpret_cast<char*>(&tx_it[i]), sizeof(uint8_t));
      }
      if(m_send_cb!=nullptr) {
	// Must extract the correct ops
	if(m_tx_ops[msg_id]==nullptr) {
	  (*m_send_cb)(m_tx_buf, msg_len, nullptr, nullptr);
	} else {
	  message_tx_ops_t* ops=(message_tx_ops_t*)m_tx_ops[msg_id]->args;
	  (*m_send_cb)(m_tx_buf, msg_len, (MessageHeader*)(&ops->header_stub), ops->args);
	}	
      }
      m_tx_entries--;
      m_tx_buf_iterator+=sizeof(std::size_t)+msg_len;
    }
  }
}
