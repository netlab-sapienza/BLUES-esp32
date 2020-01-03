#include "callbacks.hpp"



namespace bemesh{
    Master* master_istance;
    Slave* slave_istance;


    Callback::Callback(){}

    void Callback::init_callback(uint8_t type){
        switch(type){
            case SERVER:{
                master_istance = new Master();
                ESP_LOGE(FUNCTOR_TAG,"OGGETTO MASTER SULL'HEAP");
                
                //Inizializzazione e start del server.
                master_istance->start();
                return;
            }
            case CLIENT:{
                slave_istance = new Slave();
                ESP_LOGE(FUNCTOR_TAG,"OGGETTO SLAVE SULL'HEAP");
                slave_istance->start();
                return;
            }
            default:{
                ESP_LOGE(FUNCTOR_TAG, "DUNNO WHAT I'M DOING");
            }
        }
    }


    void Callback::server_update_callback( uint8_t** macs){
        master_istance->update_master_macs(macs);
    }

    void Callback::notify_callback(void){
    }

    void Callback::operator ()(){
        uint8_t ret = install_InitCb(init_callback);
        if(ret){
            ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della init_callback");
        }
        ret = install_NotifyCb(notify_callback);
        if(ret){
            ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della notify_callback");
        }

        ret = install_ServerUpdateCb(server_update_callback);
        if(ret){
           ESP_LOGE(FUNCTOR_TAG,"Errore nell'installazione della server_update_callback"); 
        }

        ESP_LOGE(FUNCTOR_TAG,"HO FINITO DI INSTALLARE LE CALLBACKS");
        return;
    }
    

}