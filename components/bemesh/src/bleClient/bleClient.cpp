#include "../include/bleClient.hpp"
#include <list>







BleClient* BleClient::getIstance(){
    if(this->istance == NULL){
        this->istance = new BleClient();
    }
    else{
        return this->istance;
    }
}

BleClient::~BleClient(){
    if(this->istance != NULL)
        delete this->istance;
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

