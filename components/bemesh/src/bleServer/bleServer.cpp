#include "../include/bleServer.hpp"




BleServer::BleServer(std::string T = "", int attempts):TAG(T),attempsUntilServer(attempts){
    this->serviceStarted = false;
    this->scanning = false;


    //TODO
    //Completare meglio quando la maggior parte delle strutture sono definite.
    this->bluetoothManager = NULL;
    this->scanCallback = NULL;
    this->onConnectionRejectedList = NULL;
    this->onDebugMessageListener=  NULL;
    this->acceptBleTask = NULL;

}



//dtor!!!
BleServer::~BleServer(){
    delete this->istance;

}



BleServer* BleServer::getIstance(){
    if(this->istance == NULL)
        this->istance = new BleServer();
    else
    {
        return this->instance;
    }
    
}


bool BleServer::isScanning(){
    return this->scanning;
}


bool BleServer::isServiceStarted(){
    return this->serviceStarted;
}


void BleServer::setScanning(bool scanning){
    this->scanning = scanning;
}



void BleServer::setServiceStarted(bool started){
    this->started = started;
}



int BleServer::getAttempsUntilServer(){
    return this->attempsUntilServer;
}


void BleServer::setRandomValueScanPeriod(int randomValueScanPeriod){
    this->randomValueScanPeriod = randomValueScanPeriod;
}



int BleServer::getRandomValueScanPeriod(){
    return this->randomValueScanPeriod;
}



std::string BleServer::getTAG(){
    return this->TAG;
}




//Complete stuffs: once AcceptBleTask is completed.





