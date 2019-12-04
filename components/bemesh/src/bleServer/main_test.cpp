 #include "../include/bleServer.hpp"
#include <iostream>

int main(){
    std::cout<<"BleServer experiment started"<<std::endl;
    BleServer* ble_server = BleServer::getIstance();

    ble_server = BleServer::getIstance();
    ble_server = BleServer::getIstance();

    ble_server->close();
    
    return 0;

}
