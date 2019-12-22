#include "services.hpp"



namespace bemesh{
    
    Attribute::Attribute(bool is_characteristic,std::string n, esp_gatt_perm_t perm )
    {
        this->is_characteristic = is_characteristic;
        name = n;
        perms = perm;
    }

    Attribute::Attribute(){
    
    }  



    RoutingTableService::RoutingTableService(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm): Attribute(is_characteristic,name,perm)
    
    {
        updatingDescriptor = 0;
        routingTable = NULL;
    }

    RoutingTableService::RoutingTableService(){
        updatingDescriptor = 0;
        routingTable = NULL;
    }

    ClientOnlineService::ClientOnlineService(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm):Attribute(is_characteristic,name,perm)

        {
        newClientOnlineDescriptor = 0;
        int i;

        for(i = 0; i<MAX_CLIENT_PER_SERVER; i++)
            notificationDescriptor[i] = 0;

        clientOnlineCharacteristic = ClientOnlineCharacteristic();
    }

    ClientOnlineService::ClientOnlineService(){

    }


    NextIdService::NextIdService(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm): Attribute(is_characteristic,name,perm)
        
    {
        nextId = 0;
    }


    NextIdService::NextIdService(){
         nextId = 0;
    }
    
    CommunicationService::CommunicationService(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm): Attribute(is_characteristic,name,perm)
        
        {
        
        clientNextIdDescriptor = 1;
        int i;

        for(i = 0; i<MAX_CLIENT_PER_SERVER; i++)
            notificationDescriptor[i] = 0;
    
        communicationCharacteristic = CommunicationCharacteristic();
    }

    CommunicationService::CommunicationService(){
         
        clientNextIdDescriptor = 1;
        int i;

        for(i = 0; i<MAX_CLIENT_PER_SERVER; i++)
            notificationDescriptor[i] = 0;
    
        communicationCharacteristic = CommunicationCharacteristic();
    }
}

