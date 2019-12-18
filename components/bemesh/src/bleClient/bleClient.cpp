#include "bleClient.hpp"
#include <list>

//Soon to be removed.
#include <iostream>

//Main issue:implementing the singleton pattern.


BleClient* BleClient::istance = NULL;

BleClient::BleClient(){
    //std::cout<<"Default ctor"<<std::endl;
    scanning = false;
    serviceStarted = false;

    //TODO. To be modified once the middle layer part is implemented.
    adapter = NULL;
    manager = NULL;
    connect_task = NULL;
    device = NULL;
    scanner = NULL;
    on_connection_listener =  NULL;
    scan_callback = NULL;

}



//Is called after BleClient::close().
BleClient::~BleClient(){
    //std::cout<<"dtor!!"<<std::endl;
}



void BleClient::close(){
    delete istance;
}



BleClient* BleClient::getIstance(){
    
    if(istance == NULL){
        std::cout<<"Element first constructed"<<std::endl;
        istance = new BleClient();
       
        return istance;
    }
    else{
        std::cout<<"Now the istance is simply returned"<<std::endl;
        
        return istance;
    }

}



void BleClient::addListener(OnClientOnlineListener list){
    this->listeners.push_back(list);

}

void BleClient::removeListener(OnClientOnlineListener list){
    this->listeners.remove(list);
}


std::list<OnClientOnlineListener> BleClient::getListeners(){
    std::list <OnClientOnlineListener>  retval (this->listeners);
    return retval;
}

