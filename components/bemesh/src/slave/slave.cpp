#include "slave.hpp"


//TO-DO. complete stuff.


namespace bemesh{
    Slave::Slave(){
        esp = true;
        connected_to_internet = false;
        device_id = 0;
        
        
        //dev_addr_t = 0;

    }

    Slave::Slave(bool is_esp, bool connected_to_internet):esp(is_esp), connected_to_internet(connected_to_internet){
        device_id = 0;

        /*
        if(is_esp)
            address = 0;
        else
        {
            //disable mac address recognition
            address = 0;
        }
         */           

    }

    Slave::~Slave(){

    }


    std::string Slave::get_name(){
        return name;
    }

    void Slave::set_name(std::string new_name){
        name = new_name;
        return;
    }


    bool Slave::is_connected_to_internet(){
        return connected_to_internet;
    }

    void Slave::set_connected_to_internet(bool connected){
        connected_to_internet = connected;
        return;
    }


    bool Slave::is_esp(){
        return esp;
    }

    void Slave::set_esp(bool is_esp){
        esp = is_esp;
        return;
    }

    dev_addr_t Slave::get_dev_addr(){
        return address;
    }

    void Slave::set_dev_addr(dev_addr_t dev_addr){
        if(esp)
            address = dev_addr;
    }


    uint16_t Slave::read_characteristic(uint16_t characteristic, dev_addr_t address,
                                void* buffer, uint16_t buffer_size)
    {

        if(buffer == NULL)
            return 0;
        if(characteristic == 0)
            return 1;     



    }

}
