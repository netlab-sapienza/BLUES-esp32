/*
 * __scanner.h
 * Handler for scanning
 */

#include "__scanner.h"

namespace bemesh {
  Scanner::Scanner(void) : m_scan_vect(){
  }

  std::vector<Node> Scanner::scan(ScanParams& t_params) {
    return m_scan_vect;
  }
}
