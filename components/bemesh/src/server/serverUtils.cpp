#include <iostream>

#include "serverUtils.hpp"

uint8_t adv_config_done = 0;
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)




namespace bemesh{
    void ServerUtils::gatts_event_handler(esp_gatts_cb_event_t callback, esp_gatt_if_t gatts_if,esp_ble_gatts_cb_param_t *param)
    {
       /* 
        If event is register event, store the gatts_if for each profile 
        if (event == ESP_GATTS_REG_EVT) {
            if (param->reg.status == ESP_GATT_OK) {
                gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
            } else {
                ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d\n",
                        param->reg.app_id,
                        param->reg.status);
                return;
            }
        }

         If the gatts_if equal to profile A, call profile A cb handler,
        * so here call each profile's callback 
        do {
            int idx;
            for (idx = 0; idx < PROFILE_NUM; idx++) {
                if (gatts_if == ESP_GATT_IF_NONE ||  ESP_GATT_IF_NONE, not specify a certain gatt_if, 
                need to call every profile cb function 
                        gatts_if == gl_profile_tab[idx].gatts_if) {
                    if (gl_profile_tab[idx].gatts_cb) {
                        gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                    }
                }
            }
        } while (0);
        */
    } 
    
    
    void ServerUtils::gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){
        
        /*
        {
        switch (event) {
            #ifdef CONFIG_SET_RAW_ADV_DATA
                case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
                    adv_config_done &= (~adv_config_flag);
                    if (adv_config_done==0){
                        esp_ble_gap_start_advertising(&adv_params);
                    }
                    break;
                case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
                    adv_config_done &= (~scan_rsp_config_flag);
                    if (adv_config_done==0){
                        esp_ble_gap_start_advertising(&adv_params);
                    }
                    break;
            #else
                //When start advertising complete, the event comes.
                case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
                    adv_config_done &= (~adv_config_flag);
                    if (adv_config_done == 0){
                        esp_ble_gap_start_advertising(&adv_params);
                    }
                    break;
                //When start scanning complete, the event comes.
                case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
                    adv_config_done &= (~scan_rsp_config_flag);
                    if (adv_config_done == 0){
                        esp_ble_gap_start_advertising(&adv_params);
                    }
                    break;
            #endif
                case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
                    //advertising start complete event to indicate advertising start successfully or failed
                    if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {

                        //ESP_BT_STATUS_SUCCESS indica condizione di successo.
                        ESP_LOGE(GATTS_TAG, "Advertising start failed\n");
                    }
                    break;
                case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
                    if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                        ESP_LOGE(GATTS_TAG, "Advertising stop failed\n");
                    } else {
                        ESP_LOGI(GATTS_TAG, "Stop adv successfully\n");
                    }
                    break;
                    //Updating dei parametri del BLE.
                case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
                    ESP_LOGI(GATTS_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                            param->update_conn_params.status,
                            param->update_conn_params.min_int,
                            param->update_conn_params.max_int,
                            param->update_conn_params.conn_int,
                            param->update_conn_params.latency,
                            param->update_conn_params.timeout);
                    break;
                default:
                    break;
                
                }
                */

    }
}   