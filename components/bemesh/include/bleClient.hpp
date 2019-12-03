
#pragma once
#include <list>

//Temporary
typedef void* OnClientOnlineListener;




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

    std::list<void*> listeners;

    //The singleton pattern istance.
    static BleClient* istance;

    BleClient();
    


    
    public:
        BleClient* getIstance();
        ~BleClient();
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
        void addListener(OnClientOnlineListener list);
        void removeListener(OnClientOnlineListener list);
        std::list<OnClientOnlineListener> getListeners();

        
        


};

