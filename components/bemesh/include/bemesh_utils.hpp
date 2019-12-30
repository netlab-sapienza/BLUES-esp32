/*
 * bemesh_utils.hpp
 * Utilities for bemesh
 */
#pragma once

#include <memory>
#include "bemesh_messages.hpp"



namespace bemesh {
  template <class T>
  class ring_buffer {
    std::unique_ptr<T[]> m_buf;
    std::size_t m_head=0;
    std::size_t m_tail=0;
    const std::size_t m_max_size;
    bool m_full=0;
  public:
    ring_buffer(std::size_t t_size);
    void put(T t_item);
    T get();
    bool empty(void) const;
    bool full(void) const;
    std::size_t capacity() const;
    std::size_t size() const;
    void reset(void);
  };
  void delete_message(MessageHeader* t_h);
}