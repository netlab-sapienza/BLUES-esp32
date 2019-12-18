#include "bleServer.hpp"
#include <iostream>




BleServer* BleServer::istance = NULL;



BleServer::BleServer(){
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
    std::cout<<"dtor!"<<std::endl;
}



BleServer* BleServer::getIstance(){
    if(istance == NULL){
        std::cout<<"First istantiation"<<std::endl;
        istance = new BleServer();
    }
    else
    {
        std::cout<<"The istance is simply returned"<<std::endl;
        return istance;
    }
    
}

void BleServer::close(){
    if(istance != NULL){
        delete istance;
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
    this->serviceStarted = started;
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





