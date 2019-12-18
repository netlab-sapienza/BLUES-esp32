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


typedef struct CommunicationMessageType{
    unsigned int from;
    unsigned int to;
    char * messageContent;
}CommunicationMessageType;


typedef struct ClientOnlineCharValue{
    //to be implemented
    void * value;
}ClientOnlineCharValue;

typedef struct Attribute{                                                                                                                     
    bool is_characteristic;                                                     
    esp_gatt_perm_t char_permissions;                                           
    std::string name;
    esp_bt_uuid_t char_uuid;                                                                     
}Attribute; 


typedef struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    Attribute* charachteristics;  
}gatts_profile_inst;



typedef struct CommunicationChar: public Attribute{
    unsigned int notificationDescriptor;
    unsigned int clientNextIdDescriptor;                                                   
    CommunicationMessageType communicationCharacteristic;
}CommunicationChar;


typedef struct RoutingTableChar: public Attribute{
    unsigned int updatingDescriptor;
    //to be added.
    void * routingTable;
}RoutingTableChar;


typedef struct NextIdChar: public Attribute{
    unsigned int nextId;
}NextIdChar;

typedef struct ClientOnlineChar: public Attribute{
    unsigned int newClientOnlineDescriptor;
    unsigned int notificationDescriptor;
    ClientOnlineCharValue clientOnlineCharacteristic;
}ClientOnlineChar;

