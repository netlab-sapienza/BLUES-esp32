

#include "utils.hpp"


namespace bemesh{
    void Utils::start_scan(void) {
        stop_scan_done = false;
        Isconnecting = false;
        uint32_t duration = 15;
        esp_ble_gap_start_scanning(duration);
    }
}