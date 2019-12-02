#include "bleClient.hpp"
#include <iostream>


BleClient* BleClient::getIstance(){
    if (this->istance == NULL){
        std::cout::<<"Building the istance"<<std::endl;
        
    }
    else
    {
        return this->istance;
    }
    
}
BleClient::BleClient(){
    this->isScanning = false;
    this->isServiceStarted= false;
}

BleClient::~BleClient(){
    
}