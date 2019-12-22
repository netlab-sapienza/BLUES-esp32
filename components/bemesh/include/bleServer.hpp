#pragma once
#include <string>
#include "server.hpp"




class BleServer{

    static BleServer* istance;
    bool serviceStarted;
    bool scanning;
    const int attempsUntilServer=400;
    int randomValueScanPeriod;
    
    std::string TAG;



    //Callback for services (integer mapping may be required).
    /*
    void nextServerIdCallback(Server* server,esp_gatts_cb_event_t event, 
                                esp_gatt_if_t gatts_if, 
                                esp_ble_gatts_cb_param_t *param);

    void communicationCallback(Server* server,esp_gatts_cb_event_t event, 
                                esp_gatt_if_t gatts_if, 
                                esp_ble_gatts_cb_param_t *param);

    */

    // map NearDeviceMap (to be implemented)

    //TODO
    // void * to be replaced with actual structures
    void* bluetoothManager;
    void* scanCallback;
    void* onEnoughServerList;
    void* onConnectionRejectedList;
    void* onDebugMessageListener;
    void* acceptBleTask;





    BleServer();

    public:
        ~BleServer();
        BleServer* getIstance();
        bool isScanning();
        bool isServiceStarted();
        void setScanning(bool scanning);
        void setServiceStarted(bool started);
        int getAttempsUntilServer();
        void setRandomValueScanPeriod(int randomValueScanPeriod);
        int getRandomValueScanPeriod();
        std::string getTAG();
        void close();


        //Listener handling function (AcceptBLETask stuff).
        void setOnDebugMessageListener(void* onDebugMessageListener);
        void addConnectionRejectedListener(void* onConnectionRejectedListener);
        void removeConnectionRejectedListener(void* onConnectionRejctedListener);
        void addRoutingTableUpdatedListener(void* routingTableUpdatedListener);
        void removeRoutingTableUpdatedListener(void* routingtTableUpdatedListener);
        void addOnMessageReceivedWithInternet(void* onMessageReceivedWithInternetListener);
        void addServerInitializedListener(void* onServerInitializedListener);
        void removeServerInitializedListener(void* onServerInitializedListener );
        std::string getId();
        void sendMessage(std::string,std::string dest, bool internet, void* onMessageSentListener);
        void addOnMessageReceivedListener(void * onMessageReceivedListener);
        void removeOnMessageReceivedListener(void* onMessageReceivedListener);
        void setEnoughServerListener(void * enoughServerListener);
        

};