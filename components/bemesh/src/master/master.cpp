#include "master.hpp"
#include <iostream>



namespace bemesh{
    void gap_event_handler_trampoline(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){}

    /*
    void Master::sayHello(){
        std::cout<<"Hello Master"<<std::endl;
    }*/

    Master::Master(uint8_t id, std::string ta):masterId(id),TAG(ta){ 
    }

    Master::Master(uint8_t id):masterId(id){
       
    }


    std::string Master::getTAG(){return TAG;}

    uint8_t Master::getId(){return masterId;}







    
}
