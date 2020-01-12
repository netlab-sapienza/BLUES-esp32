

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
		ESP_LOGE(GATTC_TAG, "BUFFER THAT TASK IS GOING TO WRITE IS:");
		esp_log_buffer_hex(GATTC_TAG, params->buffer, params->buffer_size);
		
        uint8_t conn_id = params->conn_id;
        uint16_t gatt_if = params->gatt_if;
        uint8_t charact = params->characteristic;
        uint8_t * data = params->buffer;
        uint16_t buffer_size = params->buffer_size;
        write_policy_t  policy = params->policy;
		//ESP_LOGE(GATTC_TAG,"TEST IN THE SLAVE 3: conn_id %d, gatt_if %d", conn_id, gatt_if);
        ESP_LOGE(GATTS_TAG,"In write characteristic task. policy is: %d ",policy);
        
        switch (policy){
            case Standard:{
                int i;
                uint8_t write_ret;
                for(i = 0; i<MAX_NUMBER_WRITING_ATTEMPTS; i++){
                    ESP_LOGE(GATTC_TAG,"Writing attempt number: %d",i);
                    write_ret =  write_CHR(gatt_if, conn_id, charact, data, buffer_size);
                    if(write_ret == 0){
                        //I succeded. So i break the trial loop.
                        break;
                    }
                }
                bemesh_errno = E_WRITE_CHR;
                break;
            }
            case Mandatory:{
                //Try to write until you succeed.
                while(! write_CHR(gatt_if, conn_id, charact, data, buffer_size));
                break;
            }
            default:{
                ESP_LOGE(GATTC_TAG,"Unknown write policy");
                break;
            }
        }
        
		delete params;
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


