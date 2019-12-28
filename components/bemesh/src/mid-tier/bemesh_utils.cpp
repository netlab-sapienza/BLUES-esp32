/*
 * bemesh_utils.cpp
 * Utilities for bemesh
 */

#include "bemesh_utils.hpp"

namespace bemesh {

  template <typename T>
  ring_buffer<T>::ring_buffer(std::size_t t_size):
    m_buf(std::unique_ptr<T[]>(new T[t_size])), m_max_size(t_size){
    //empty constructor :)
  }

  template <typename T>
  void ring_buffer<T>::reset(void) {
    m_head=m_tail;
    m_full=false;
  }

  template <typename T>
  bool ring_buffer<T>::empty(void) const {
    return (!m_full && (m_head==m_tail));
  }

  template <typename T>
  bool ring_buffer<T>::full(void) const {
    return m_full;
  }

  template <typename T>
  std::size_t ring_buffer<T>::capacity() const {
    return m_max_size;
  }
  
  template <typename T>
  std::size_t ring_buffer<T>::size() const {
    std::size_t size = m_max_size;
    if(!m_full) {
      if(m_head >= m_tail) {
	size = m_head-m_tail;
      } else {
	size = m_max_size + m_head - m_tail;
      }
    }
    return size;
  }
  
  template <typename T>
  void ring_buffer<T>::put(T t_item) {
    // place the item in the buffer
    m_buf[m_head]=t_item;
    if(m_full) {
      // shift forward the tail to empty a place
      m_tail = (m_tail+1)%m_max_size;
    }
    m_head=(m_head+1)%m_max_size;
    m_full = (m_head==m_tail);
  }

  template <typename T>
  T ring_buffer<T>::get() {
    assert(!empty()&&"get on an empty ring buffer");

    // read data
    auto val=m_buf[m_tail];
    m_full=false;
    m_tail = (m_tail+1)%m_max_size;
    return val;
  }


  void delete_message(MessageHeader* t_h) {
    std::size_t message_size=MESSAGE_SIZE(t_h);
    uint8_t *h=(uint8_t*)t_h;
    delete[] h;
    return;
  }
}
