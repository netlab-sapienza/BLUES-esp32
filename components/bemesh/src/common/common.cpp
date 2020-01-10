

#include "common.hpp"


namespace bemesh{
    task_param_write_t::task_param_write_t(){}

    connected_server_params_t::connected_server_params_t(){}

    connected_server_params_t::connected_server_params_t(uint8_t server_id, uint16_t gatt_if,
                                                uint8_t conn_id, dev_addr_t mac_address): 
                                                server_id(server_id),gatt_if(gatt_if),
                                                conn_id(conn_id),
                                                server_mac_address(mac_address){
                                                }

    

    bool connected_server_params_t::operator ==(const connected_server_params_t& cc){
        return server_id == cc.server_id && gatt_if == cc.gatt_if && conn_id && cc.conn_id &&
                    same_addresses(server_mac_address,cc.server_mac_address,MAC_ADDRESS_SIZE);
    }

    bool connected_server_params_t::operator !=(const connected_server_params_t& cc){
        return !operator==(cc);
    }


    ping_data_t::ping_data_t(){}

    ping_data_t::ping_data_t(dev_addr_t addr, uint8_t flag,uint8_t c, uint16_t g):sender(addr),pong_flag(flag),
                                        conn_id(c),gatt_if(g){}

    bool  ping_data_t::operator ==(const ping_data_t& cc){
        return cc.pong_flag == pong_flag && same_addresses(sender,cc.sender,MAC_ADDRESS_SIZE);
    }

    bool ping_data_t::operator !=(const ping_data_t & cc){
        return !operator==(cc);
    }


    bool contains_server(std::list<connected_server_params_t>ls,uint8_t id){
        std::list<connected_server_params_t>::iterator it;
        for(it = ls.begin(); it!= ls.end(); ++it){
            connected_server_params_t params= *it;
            if(params.server_id == id)
                return true;
        }
        return false;
    }

    bool contains_mac(std::list<uint8_t*> ls,uint8_t* address, uint8_t address_size){
        std::list<uint8_t*>::iterator it;
        for(it = ls.begin(); it != ls.end(); ++it){
            uint8_t * addr = *it;
            int i;
            bool found = true;
            for(i = 0;i<address_size; i++){
                if(addr[i] != address[i])
                    found = false;
            }
            if(found)
                return true;
        }
        return false;

    }



    dev_addr_t _build_dev_addr(uint8_t* address){
        int i;
        dev_addr_t ret;
        for(i = 0; i<DEV_ADDR_LEN; ++i){
            ret[i] = address[i];
        }
        return ret;
    }
    void _print_mac_address(uint8_t* address){
        int i;
        for(i = 0; i<MAC_ADDRESS_SIZE;i++){
            ESP_LOGE("CLIENT","Byte[%d]: %x",i,address[i]);
        }

    }


    //A task used to perform writing on charateristics. In esp-idf task ~ thread.
    void write_characteristic_task(void * task_param) {
        task_param_write_t* params = (task_param_write_t*)task_param;
    
        uint8_t conn_id = params->conn_id;
        uint16_t gatt_if = params->gatt_if;
        uint8_t charact = params->characteristic;
        uint8_t * data = params->buffer;
        uint16_t buffer_size = params->buffer_size;
        
        write_CHR(gatt_if, conn_id, charact, data, buffer_size);
        //std::cout<<"I wrote"<<std::endl;
    /* uint8_t * test = read_CHR(gatt_if,conn_id, charact);
        int i;
        for(i = 0; i<buffer_size; i++){
            ESP_LOGE(GATTS_TAG,"Found: %d",test[i]);
        }
    */
        vTaskDelete(NULL);

    }
    
    bool same_addresses(dev_addr_t addr1,dev_addr_t addr2, uint8_t address_size){
        bool ret = true;
        int i;
        for(i = 0; i< address_size; i++){
            if(addr1[i] != addr2[i]){
                ret = false;
                return ret;
            }
        }
        return ret;
    }



}


