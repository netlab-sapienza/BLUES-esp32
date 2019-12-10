/*
 * __nodes.h
 * 
 */
#pragma once

#include "__status.h"
#include <stdint.h>

namespace bemesh {
  struct Node {
    uint8_t m_id;
    NodeStatus m_status;
    Node(uint8_t t_id, NodeStatus t_status);
  };
}
