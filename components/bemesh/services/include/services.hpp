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
#define NUM_CHARACTERISTICS 4


namespace bemesh{
    struct CommunicationCharValue{
        unsigned int from;
        unsigned int to;
        char * message_content;
        CommunicationCharValue();
    };


    struct ClientOnlineCharValue{
        //to be implemented
        void * value;
        ClientOnlineCharValue();
    };


    class Attribute{
        bool is_characteristic;
        esp_gatt_perm_t perms;
        std::string name;
        public:
            void set_name(std::string name);
            std::string get_name();
            void set_perms(esp_gatt_perm_t perms);
            esp_gatt_perm_t get_perms();
            bool characteristic();
            void set_characteristic_status(bool characteristic_status);
            Attribute();
            Attribute(bool is_char,std::string na, esp_gatt_perm_t per);
    };



    struct gatts_profile_inst {
        esp_gatts_cb_t gatts_cb;
        uint16_t gatts_if;
        uint16_t app_id;
        uint16_t conn_id;
        uint16_t service_handle;
        esp_gatt_srvc_id_t service_id;
        uint16_t char_handle;
        esp_bt_uuid_t char_uuid;
        esp_gatt_perm_t perm;
        esp_gatt_char_prop_t property;
        uint16_t descr_handle;
        esp_bt_uuid_t descr_uuid;
    };

   
    class CommunicationCharacteristic: public Attribute{
        unsigned int notification_descriptor[MAX_CLIENT_PER_SERVER];
        unsigned int client_next_id_descriptor;
        CommunicationCharValue char_data;
        public:
            unsigned int get_client_next_id_descriptor();
            CommunicationCharValue get_char_data();
            void set_char_data(unsigned int from, unsigned int to, char* message_value);
            CommunicationCharacteristic();
            CommunicationCharacteristic(bool is_characteristic, std::string name,
                                        esp_gatt_perm_t perm);
        
    };


    class RoutingTableCharacteristic: public Attribute{
        unsigned int updating_descriptor;
        void* routing_table;
        public:
            void* get_routing_table();
            //void modify_routing_table;
            RoutingTableCharacteristic(bool is_characteristic, std::string name,
                                        esp_gatt_perm_t perm);
            RoutingTableCharacteristic();
    };

    class NextIdCharacteristic: public Attribute{
        unsigned int next_id;
        public:
            unsigned int get_next_id();
            void increment_id();
            void set_next_id(unsigned int id);
            NextIdCharacteristic(bool is_characteristic, std::string name,
               esp_gatt_perm_t perms);
            NextIdCharacteristic();
    };

    class ClientOnlineCharacteristic: public Attribute{
        unsigned int new_client_online_descriptor;
        unsigned int notification_descriptor[MAX_CLIENT_PER_SERVER];
        ClientOnlineCharValue char_data;
        public:
            ClientOnlineCharValue get_char_data();
            void set_char_data(void* data);
            ClientOnlineCharacteristic(bool is_characteristic, std::string name,
                                        esp_gatt_perm_t perms);
            ClientOnlineCharacteristic();


    };

     class Service{
        public:
            esp_gatts_cb_t cbks[NUM_CHARACTERISTICS];
            uint16_t gatts_if;
            uint16_t app_id;
            uint16_t conn_id;
            esp_gatt_srvc_id_t service_id;
            //characteristics;
            CommunicationCharacteristic comm_char;
            RoutingTableCharacteristic routing_char;
            ClientOnlineCharacteristic client_online_char;
            NextIdCharacteristic next_id_char;

            //Default ctor.
            Service();


        
    };

    

}