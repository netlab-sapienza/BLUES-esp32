#include "master.hpp"
#include <iostream>



namespace bemesh{
  

    Master::Master(uint8_t id, std::string name):name(name),master_id(id){ 
    }

    Master::Master(uint8_t id):master_id(id){
       
    }

    Master::~Master(){

    }


    std::string Master::get_name(){
        return name;
    }

    void Master::set_name(std::string new_name){
        name = new_name;
        return;
    }

    uint8_t Master::get_id(){
        return master_id;
    }

    void Master::set_id(uint8_t id){
        master_id = id;
        return;
    }


     bool Master::is_connected_to_internet(){
        return connected_to_internet;
    }

    void Master::set_connected_to_internet(bool connected){
        connected_to_internet = connected;
        return;
    }


    bool Master::is_esp(){
        return esp;
    }

    void Master::set_esp(bool is_esp){
        esp = is_esp;
        return;
    }

    dev_addr_t Master::get_dev_addr(){
        return address;
    }

    void Master::set_dev_addr(dev_addr_t dev_addr){
        if(esp)
            address = dev_addr;
    }



    






    
}
