#include "services.hpp"



namespace bemesh{



    ClientOnlineCharValue::ClientOnlineCharValue(){

    }

    CommunicationCharValue::CommunicationCharValue(){

    }
    
    Attribute::Attribute(bool is_characteristic,std::string name, esp_gatt_perm_t perms )
    {
        this->is_characteristic = is_characteristic;
        this->name = name;
        this->perms = perms;
    }

    Attribute::Attribute(){
    
    }  


    void Attribute::set_characteristic_status(bool characteristic_status){
        this->is_characteristic = characteristic_status;
    }

    bool Attribute::characteristic(){
        return is_characteristic;
    }

    void Attribute::set_name(std::string name){
        this->name = name;
    }
    
    std::string Attribute::get_name(){
        return name;
    }

    void Attribute::set_perms(esp_gatt_perm_t perms){
        this->perms = perms; 
    }

    esp_gatt_perm_t Attribute::get_perms(){
        return perms;
    }


    RoutingTableCharacteristic::RoutingTableCharacteristic(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm): Attribute(is_characteristic,name,perm)
    
    {
        updating_descriptor = 0;
        routing_table = NULL;
    }

    RoutingTableCharacteristic::RoutingTableCharacteristic(){
        updating_descriptor = 0;
        routing_table = NULL;
    }

    ClientOnlineCharacteristic::ClientOnlineCharacteristic(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm):Attribute(is_characteristic,name,perm)

        {
        new_client_online_descriptor = 0;
        int i;

        for(i = 0; i<MAX_CLIENT_PER_SERVER; i++)
            notification_descriptor[i] = 0;


        //Initialize the default value 
        char_data.value = NULL;
    }

    ClientOnlineCharacteristic::ClientOnlineCharacteristic(){

    }


    NextIdCharacteristic::NextIdCharacteristic(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm): Attribute(is_characteristic,name,perm)
        
    {
        next_id = 0;
    }


    NextIdCharacteristic::NextIdCharacteristic(){
         next_id = 0;
    }
    
    CommunicationCharacteristic::CommunicationCharacteristic(bool is_characteristic,
        std::string name, esp_gatt_perm_t perm): Attribute(is_characteristic,name,perm)
        
        {
        
        client_next_id_descriptor = 1;
        int i;

        for(i = 0; i<MAX_CLIENT_PER_SERVER; i++)
            notification_descriptor[i] = 0;

        //Default initialization for the data part of the characteristic.
        char_data.from = 0;
        char_data.to = 0;
        char_data.message_content = NULL;
    }

    CommunicationCharacteristic::CommunicationCharacteristic(){
         
        client_next_id_descriptor = 1;
        int i;

        for(i = 0; i<MAX_CLIENT_PER_SERVER; i++)
            notification_descriptor[i] = 0;
    
        char_data.from = 0;
        char_data.to = 0;
        char_data.message_content = NULL;
    }

    unsigned int CommunicationCharacteristic::get_client_next_id_descriptor(){
        return client_next_id_descriptor;
    }


    Service::Service(){

        //Defining default permissions and values for base characteristics.
        std::string communication_name = "communication";
        std::string next_id_name = "next_id";
        std::string client_online_name = "client_online";
        std::string routing_table_name = "routing_table";

        comm_char.set_name(communication_name);
        client_online_char.set_name(client_online_name);
        next_id_char.set_name(next_id_name);
        routing_char.set_name(routing_table_name);


        esp_gatt_perm_t comm_perms, next_id_perms, client_online_perms, routing_perms;
        uint16_t read_perms = 0 | ESP_GATT_PERM_READ;
        uint16_t write_perms = 0 | ESP_GATT_PERM_WRITE;

        //Setting permissions for each characteristic. By default everyone can read and write from those
        //characteristics.

        comm_perms = 0 | read_perms | write_perms;
        next_id_perms = 0 |read_perms | write_perms;
        routing_perms = 0 |read_perms | write_perms;
        client_online_perms = 0 |read_perms | write_perms;
        
        comm_char.set_perms(comm_perms);
        client_online_char.set_perms(client_online_perms);
        routing_char.set_perms(routing_perms);
        next_id_char.set_perms(next_id_perms);






        
    }
}

