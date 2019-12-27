#include "rtable.hpp" //For dev_addr_t data type.
#include <stdint.h>
#include <string>

namespace bemesh{
    class Slave{
        dev_addr_t address;
        bool esp;
        bool connected_to_internet;
        std::string name;


        uint8_t device_id; //for android compatibility mode.


        public:
            Slave();
            ~Slave();
            Slave(bool is_esp, bool connected_to_internet);

            std::string get_name();
            void set_name(std::string name);

            bool is_connected_to_internet();
            void set_connected_to_internet(bool connected_to_internet);

            bool is_esp();
            void set_esp(bool is_esp);

            dev_addr_t get_dev_addr();
            void set_dev_addr(dev_addr_t new_dev_addr);



            uint16_t read_characteristic(uint16_t characteristic, dev_addr_t address,void* buffer,uint16_t char_size);






        


    };
}