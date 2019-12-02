
#pragma once
#include <vector>

class BleClient{
    const int handlerPeriod = 5000;
    bool scanning;
    bool serviceStarted;
    char lastServerIdFound [2];



    //Da sostituire con le classi effettive.
    void* adapter;
    void* manager;
    void* connect_task;
    void* device;
    void* scanner;
    void* on_connection_listener;
    void* scan_callback;


    //The singleton pattern istance.
    
    
    public:
        void* getBluethoothAdapter(){return adapter;}
        void* getBluethoothManager(){return manager;}
        void* getConnectBleTask(){return connect_task;}
        void* getBluethoothDevice(){return device;}
        void* getBluethoothLeScanner(){return scanner;}
        void* getOnConnectionListener(){return on_connection_listener;}
        void* getServerScanCallback(){return scan_callback;}
        bool isScanning(){return scanning;}
        bool isServiceStarted(){return serviceStarted;}
        int getHandlerPeriod(){return handlerPeriod;}
        void setScanning(bool scann){this->scanning = scann;}
        void setServiceStarted(bool service){this->serviceStarted = service;}
        char[2] getLastServerIdFound(){return lastServerIdFound; }

        
        


};

