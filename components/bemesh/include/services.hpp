#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"


//
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"

#define MAX_CLIENT_PER_SERVER 7



namespace bemesh{
    struct CommunicationCharacteristic{
        unsigned int from;
        unsigned int to;
        char * messageContent;
        CommunicationCharacteristic();
    };


    struct ClientOnlineCharacteristic{
        //to be implemented
        void * value;
        ClientOnlineCharacteristic();
    };

    struct Attribute{                                                                                                                     
        bool is_characteristic;                                                     
        esp_gatt_perm_t perms;                                           
        std::string name;  
        Attribute(bool is_char, std::string na, esp_gatt_perm_t per);                                                                  
    }; 


    struct gatts_profile_inst {
        esp_gatts_cb_t gatts_cb;
        uint16_t gatts_if;
        uint16_t app_id;
        uint16_t conn_id;
        uint16_t service_handle;
        esp_gatt_perm_t perm;
        esp_gatt_char_prop_t property;
        Attribute* charachteristics;  
    };



    struct CommunicationService: public Attribute{
        unsigned int notificationDescriptor[MAX_CLIENT_PER_SERVER];
        unsigned int clientNextIdDescriptor;                                                   
        CommunicationCharacteristic communicationCharacteristic;
        CommunicationService(bool, std::string, esp_gatt_perm_t);
    };


    struct RoutingTableService: public Attribute{
        unsigned int updatingDescriptor;
        //to be added.
        void * routingTable;
        RoutingTableService(bool, std::string, esp_gatt_perm_t);
    };


    struct NextIdService: public Attribute{
        unsigned int nextId;
        NextIdService(bool, std::string, esp_gatt_perm_t);
    };

    struct ClientOnlineService: public Attribute{
        unsigned int newClientOnlineDescriptor;

        //This service is used by servers. We may want that more servers are notified that
        //a client is online.
        unsigned int notificationDescriptor[MAX_CLIENT_PER_SERVER];
        ClientOnlineCharacteristic clientOnlineCharacteristic;
        ClientOnlineService(bool,std::string,esp_gatt_perm_t);
    };

}