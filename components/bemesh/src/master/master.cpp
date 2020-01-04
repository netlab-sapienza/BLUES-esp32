#include "master.hpp"
#include <iostream>



namespace bemesh{
  

    Master::Master(uint8_t id, std::string name):name(name),device_conn_id(id),
    connected_clients()
    { 
    }

    Master::Master(uint8_t id):device_conn_id(id),connected_clients(){
    }

    Master::Master():connected_clients(){
        
    }

    

    Master::~Master(){

    }


    std::string Master::get_name(){
        return name;
    }

    void Master::set_name(std::string new_name){
        name = new_name;
        return;
    }


     bool Master::is_connected_to_internet(){
        return connected_to_internet;
    }

    void Master::set_connected_to_internet(bool connected){
        connected_to_internet = connected;
        return;
    }


    bool Master::is_esp(){
        return esp;
    }

    void Master::set_esp(bool is_esp){
        esp = is_esp;
        return;
    }

    dev_addr_t Master::get_dev_addr(){
        return address;
    }

    void Master::set_dev_addr(dev_addr_t dev_addr){
        if(esp)
            address = dev_addr;
        return;
    }


    uint16_t Master::get_device_connection_id(){
        return device_conn_id;
    }

    void Master::set_device_connection_id(uint16_t conn_id){
        device_conn_id = conn_id;
    }

    uint8_t Master::get_device_gatt_if(){
        return device_gatt_if;
    }

    void Master::set_device_gatt_if(uint16_t gatt_if){
        device_gatt_if = gatt_if;
    }

    MessageHandler* Master::get_message_handler(){
        return &mes_handler;
    }

    Router* Master::get_router(){
        return router;
    }

    uint8_t* Master::get_master_tx_buffer(){
        return master_tx_buffer;
    }


    //Puntatore alla matrice.
    uint8_t** Master::get_connected_devices_macs(){
        return connected_devices_macs;
    }

    uint8_t* Master::get_connected_devices_conn_id(){
        return connected_devices_conn_id;
    }

    void Master::add_routing_table_entry(dev_addr_t target_addr, dev_addr_t hop_addr,
                            uint8_t num_hops, uint8_t t_flags)
    {
        router->add(target_addr,hop_addr, num_hops,t_flags);                    
    }

    void Master::remove_routing_table_entry(dev_addr_t target_addr){
        router->remove(target_addr);
    }

    dev_addr_t& Master::get_next_hop(dev_addr_t addr){
        return router->nextHop(addr);
    }

    void Master::add_connected_client(uint8_t* new_address){
        //Implement a contains check
        connected_clients.push_back(new_address);
    }

    void Master::remove_connected_client(uint8_t* address ){
        //Implement a contains check.
        connected_clients.remove(address);
    }

    std::list<uint8_t*> Master::get_connected_clients(){
        std::list<uint8_t*> ret(connected_clients);
        return ret;
    }

    dev_addr_t& Master::get_router_dev_addr(){
        return router->addr();
    }




    //Master object main task. Used to test functions and primitives.
    void Master::start(){

        //Can't start if master is not allocated.
        if(master_instance == NULL)
            return;

        //For now there will only be esp.They won't send data to internet.
        set_esp(true);
        set_connected_to_internet(false);

        uint16_t  gatt_if = get_gatt_if();
        uint8_t* mac_address = get_my_MAC();
        uint8_t conn_id = get_client_connid();
        uint8_t** devices = get_connected_MACS();
        connected_devices_macs = devices;
        


        dev_addr_t addr;
        if(mac_address != NULL){
            addr = _build_dev_addr(mac_address);
            //Router object allocation.
            router = new Router(addr);
        }
        
        set_device_gatt_if(gatt_if);
        set_device_connection_id(conn_id);
        set_dev_addr(addr);
        

        //Buffer to send/receive messages.
        mes_handler.installTxBuffer(master_tx_buffer);
        //Routing mechanism
        //Next hop di un client è sempre un server.


    }



    int16_t Master::read_characteristic(uint8_t characteristic, dev_addr_t address,void* buffer,
                                        uint16_t buffer_size, uint16_t gattc_if,
                                        uint16_t conn_id)
    {
        if(buffer == NULL)
            return -1;
        if(characteristic == IDX_CHAR_A || characteristic == IDX_CHAR_B || 
            characteristic == IDX_CHAR_C)
        {

            uint8_t* received_bytes=  read_CHR(gattc_if,conn_id,characteristic);
            uint8_t char_len_read = get_CHR_value_len(characteristic);

            if(buffer_size < char_len_read){
                return -1;
            }

            memcpy((void*)received_bytes,buffer,char_len_read);
            return char_len_read;

        }
        else
            return -1;    

    }




    ErrStatus Master::write_characteristic(uint8_t characteristic, dev_addr_t address, void* buffer,
                                        uint8_t buffer_size, uint16_t gatts_if,uint16_t conn_id)
    {
        if(buffer == NULL)
            return WrongPayload;
        if(conn_id != device_conn_id)
            return WrongAddress;

        if(characteristic == IDX_CHAR_A || characteristic == IDX_CHAR_B ||
            characteristic == IDX_CHAR_C )
        {
            write_CHR(gatts_if,conn_id,characteristic,(uint8_t*)buffer,buffer_size);
            return Success;
        }        
        
        else
        {
            return GenericError;
        }


    }

    void Master::shutdown(){
        if(master_instance != NULL)
            delete master_instance;
    }
    
    /*
        Private functions
    */
    dev_addr_t Master::_build_dev_addr(uint8_t* address){
        int i;
        dev_addr_t ret;
        for(i = 0; i<DEV_ADDR_LEN; ++i){
            ret[i] = address[i];
        }
        return ret;
    }
    void Master::_print_mac_address(uint8_t* address){
        
        int i;
        for(i = 0; i<MAC_ADDRESS_SIZE;i++){
           ESP_LOGE("CLIENT","Byte[%d]: %x",i,address[i]);
        }
    }

    //Applicazione che mette in atto il processo di sincronizzazione delle routing table.
    //Interfaccia che permette di inviare i comandi.
    //Nodi che forwardano la routing table.

    //Function that updates the client connected table.
    void Master::update_master_macs(uint8_t* address){
        std::cout<<"Updating client table"<<std::endl;
        if(address){
            add_connected_client(address);
            std::cout<<"New address is:"<<std::endl;
            _print_mac_address(address);
            dev_addr_t addr = _build_dev_addr(address);
            uint8_t hops = 1;
            uint8_t flags = 0;
            dev_addr_t my_addr = get_router_dev_addr();
            add_routing_table_entry(addr,my_addr,hops,flags);
            std::cout<<"Added an entry to the routing table: "<<std::endl;
        } 
        
    }

   
}
