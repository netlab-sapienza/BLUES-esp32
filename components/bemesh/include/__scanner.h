/*
 * __scanner.h
 * Handler for scanning
 */

#pragma once

#include "__status.h"
#include "__nodes.h"
#include <vector>

namespace bemesh {
  struct ScanParams {
    //TODO
    uint8_t todo;
  };
  
  struct Scanner {
    // Result of scan as a vector of found nodes
    // Refer to the __node header for additional
    // information 
    std::vector<Node> m_scan_vect;
    // Scan parameters
    // Should contain Bluedroid parameters too
    ScanParams m_params;
    Scanner(void);
    //TODO: This call should be blocking ? 
    std::vector<Node> scan(ScanParams& t_params);
  };
}
