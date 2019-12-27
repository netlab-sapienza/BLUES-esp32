#include "rtable.hpp" //For dev_addr_t data type.
#include <stdint.h>

namespace bemesh{
    class Slave{
        unsigned int id;
        bool connected_to_internet;

        
        uint16_t read_characteristic(uint16_t characteristic, dev_addr_t address,void* buffer,uint16_t char_size);






        


    };
}