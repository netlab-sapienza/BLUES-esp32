/*
 * gap_handler.c
 * Handler for GAP operations
 */

#include "gap_handler.h"
#include "esp_log.h" // Logging library
#include "nvs_flash.h"
#include <string.h> // memcpy

static const char* TAG = "gap_handler";

// Define the service that ESP will advert.
uint8_t bemesh_gap_service_uuid[GAP_SRV_UUID_LEN] = {
  0xca, 0xd8, 0x1e, 0xed, 0xd2, 0x9b, 0x4a, 0x84,
  0x91, 0x8e, 0xc1, 0xf7, 0xc6, 0x93, 0x60, 0x9b
};

// Since we want one and only one gap_handler we will statically define it.
static bemesh_gap_handler gap1;
static bemesh_gap_handler *get_gap1_ptr(void) {
  return &gap1;
}

// GAP callback handler definition
static void bemesh_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/*
 * Configure the advertising data for the module.
 * The current configuration for the advertising data packet
 * is the following:
 * -name: no
 * -min_interval: 7.5ms
 * -max_interval: 20ms
 * -appearance: 0x00
 * -no manufacturer data
 * -no service data
 */
static void setup_advertising_data(esp_ble_adv_data_t* adv,
				   bemesh_gap_handler* h,
				   uint8_t* srv_uuid_buffer,
				   uint8_t srv_uuid_len) {
  adv->set_scan_rsp=false; // This is not the response structure.
  adv->include_name=false; // Do not include the name in the adv packet.
  adv->min_interval=0x06; // Minimum advertising interval : 0x06 * 1.25ms = 7.5 ms.
  adv->max_interval=0x10; // Maximum advertising interval : 0x10 * 1.25ms = 20 ms.
  adv->appearance=0x00; // standard appearance value

  adv->manufacturer_len=0;
  adv->p_manufacturer_data=NULL;

  adv->service_data_len=0;
  adv->p_service_data=NULL;
  // TODO: Setup the service uuid payload.
  adv->service_uuid_len=srv_uuid_len;
  adv->p_service_uuid=srv_uuid_buffer;

  adv->flag = (ESP_BLE_ADV_FLAG_GEN_DISC |
	       ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
  return;
}

// Configure the response data for the module.
static void setup_resp_data(esp_ble_adv_data_t* adv,
			    bemesh_gap_handler* h) {
  adv->set_scan_rsp=true; // This is the response structure.
  adv->include_name=false; // Do not include the name in the resp packet.

  // Will transmit data present in the rsp_man_buffer.
  // TAKE CARE: maximum payload size of 31 bytes for the resp.
  adv->manufacturer_len=h->rsp_man_buffer_len;
  adv->p_manufacturer_data=h->rsp_man_buffer;

  adv->service_data_len=0;
  adv->p_service_data=NULL;

  adv->service_uuid_len=0;
  adv->p_service_uuid=NULL;
  return;
}

/*
 * Configure the advertising parameters for the module.
 * -min_interval: 20ms
 * -max_interval: 40ms
 * -static BDA (MAC dependent)
 * -generic advertisement (ADV_TYPE_IND)
 * -use of all channels
 * -allow both scan and connection requests from any central.
 */
static void setup_advetising_params(esp_ble_adv_params_t* params,
				    bemesh_gap_handler* h) {
  params->adv_int_min=0x20;
  params->adv_int_max=0x40;
  params->adv_type=ADV_TYPE_IND;
  params->own_addr_type=BLE_ADDR_TYPE_PUBLIC;
  //params->peer_addr;
  //params->peer_addr_type;
  params->channel_map=ADV_CHNL_ALL;
  params->adv_filter_policy=ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
  return;
}

/*
 * Configure the scanning parameters for the module.
 * -Active scan [Includes reading scan response data]
 * -Public BDA in use
 * -Open scan filter policy
 * -scan interval of 50ms
 * -scan window of 30ms
 * each scan operation is 30ms long, and repeats every 50ms
 */
// TODO delete magic numbers
static void setup_scanning_params(esp_ble_scan_params_t* params,
				  bemesh_gap_handler* h) {
            params->scan_type=BLE_SCAN_TYPE_ACTIVE;
            params->own_addr_type=BLE_ADDR_TYPE_PUBLIC;
            params->scan_filter_policy=BLE_SCAN_FILTER_ALLOW_ALL;
            params->scan_interval=0x50; // TODO set the value
            params->scan_window=0x30; // TODO set the value
            return;
    }

// Initializes the ble structures in the gap handler h.
bemesh_gap_handler* bemesh_gap_handler_init(uint8_t* rsp_buffer,
					    uint8_t rsp_buffer_len,
					    uint8_t *srv_uuid_buffer,
					    uint8_t srv_uuid_len) {
  // SET LOGGING LEVEL TO WARNING
  //esp_log_level_set(TAG, ESP_LOG_WARN);
  
  // set callback function
  ESP_ERROR_CHECK(esp_ble_gap_register_callback(bemesh_gap_cb));
  
  // get gap1 ptr
  bemesh_gap_handler* h=get_gap1_ptr();
  // Setup the response manufacturer buffer
  h->rsp_man_buffer=rsp_buffer;
  h->rsp_man_buffer_len=rsp_buffer_len;
  // Setup the structures of gap handler
  //setup_advertising_data(&h->adv_data, h, srv_uuid_buffer, srv_uuid_len); // adv data
  setup_advertising_data(&h->adv_data, h, bemesh_gap_service_uuid, GAP_SRV_UUID_LEN); // adv data
  setup_resp_data(&h->rsp_data, h); // rsp data
  setup_advetising_params(&h->adv_params, h); // adv params
  setup_scanning_params(&h->scan_params, h); // scan params

  // initialize the handler's vars
  h->scan_params_complete=0;
  h->found_devs=0;
  h->adv_data_set=false;
  h->rsp_data_set=false;

  // Execute paramter passing routines to esp
  ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&h->scan_params));
  ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&h->adv_data));
  ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&h->rsp_data));  
  return h;
}

// Configure the operational mode on the gap handler h.
void bemesh_gap_handler_mode(bemesh_gap_handler* h, uint8_t m) {
  h->mode=m;
  if(m==GAP_HANDLER_MODE_PERIPHERAL) {
    ESP_LOGI(TAG, "Starting peripheral mode.");
    // Procedure to become a peripheral (advertising)
    // if the device was scanning, stop it.
    esp_ble_gap_stop_scanning();
    // if advertising and scan response setup proc. was completed,
    // start advertising
    // wait for data setup.
    while(!h->rsp_data_set) {}
    if(h->adv_data_set==true && h->rsp_data_set==true) {
      esp_ble_gap_start_advertising(&h->adv_params);
    }
  } else if(m==GAP_HANDLER_MODE_CENTRAL) {
    ESP_LOGI(TAG, "Starting central mode.");
    // Procedure to become a central (scanning)
    // if the device was advertising, stop it.
    esp_ble_gap_stop_advertising();
    // if the scan parameters setup proc. was completed, start scanning
    if(h->scan_params_complete) {
      esp_ble_gap_start_scanning(SCAN_DURATION_SEC);
    }
  }
  return;
}

//TODO add docs
int bemesh_gap_handler_start_scanning(bemesh_gap_handler* h, uint8_t timeout) {
  if(h->flags&O_ADV) {
    // Cannot start scanning if the module is advertising
    ESP_LOGE(TAG, "Error: could not start scan proc. The module is advertising");
    return -1;
  }
  // Reset the number of found devices.
  h->found_devs = 0;
  // Start scanning proc. and set the correct flag
  esp_ble_gap_start_scanning(timeout);
  h->flags&=~O_SCNCMPL; // Reset the scan complete flag.
  h->flags|=O_SCN; // set O_SCN to true.
  return 0;
}
//TODO add docs
void bemesh_gap_handler_stop_scanning(bemesh_gap_handler* h) {
  if(!(h->flags&O_SCN)) {
    ESP_LOGV(TAG, "Warning: could not stop scan proc. The module is not scanning.");
    return;
  }
  // Stop scanning proc.
  esp_ble_gap_stop_scanning();
  h->flags&=~O_SCN; // Reset the scan flag
  h->flags|=O_SCNCMPL; // Set scan complete flag
  return;
}
//TODO add docs
uint8_t bemesh_gap_handler_scan_complete(bemesh_gap_handler* h) {
  return h->flags&O_SCNCMPL;
}
//TODO add docs
int bemesh_gap_handler_start_advertising(bemesh_gap_handler* h) {
  if(h->flags&O_SCN) {
    // Cannot start advertising if the module is scanning
    ESP_LOGE(TAG, "Error: could not start adv proc. The module is scanning");
    return -1;
  }
  // Start adv proc. and set the correct flag.
  esp_ble_gap_start_advertising(&h->adv_params);
  h->flags|=O_ADV;
  return 0;
}
//TODO add docs
void bemesh_gap_handler_stop_advertising(bemesh_gap_handler* h) {
  if(!(h->flags&O_ADV)) {
    ESP_LOGV(TAG, "Warning: could not stop adv proc. The module is not advertising.");
    return;
  }
  esp_ble_gap_stop_advertising();
  h->flags&=~O_ADV;
  return;
}
//TODO add docs
uint8_t bemesh_gap_handler_get_scan_res_len(bemesh_gap_handler* h) {
  return h->found_devs;
}
//TODO add docs
bemesh_dev_t *bemesh_gap_handler_get_scan_res(bemesh_gap_handler* h) {
  return h->found_devs_vect;
}

void bemesh_gap_handler_install_cb(bemesh_gap_handler *h, kernel_cb cb, bemesh_evt_params_t *params) {
  h->core_cb=cb;
  h->core_cb_args=params;
  return;
}
void bemesh_gap_handler_uninstall_cb(bemesh_gap_handler *h) {
  h->core_cb=NULL;
  h->core_cb_args=NULL;
}


// CENTRAL MODE callbacks (scan)
static void scan_param_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
static void scan_start_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
static void scan_result_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);

// PERIPHERAL MODE callbacks (adv)
static void adv_data_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
static void adv_rsp_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
static void adv_start_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);

// General cases (i think so ?)
static void conn_params_update_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);

/*
 * GAP Event handler.
 * Executed when a new GAP event occurs.
 */
static void bemesh_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  bemesh_gap_handler* h=get_gap1_ptr();
  switch(event) {
  case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
    // Scan parameters setup complete. (First event)
    scan_param_complete_cb(param, h);
    break;
  case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
    // Scan start procedure
    scan_start_complete_cb(param, h);
    break;
  case ESP_GAP_BLE_SCAN_RESULT_EVT:
    // Received new results from scan.
    scan_result_cb(param, h);
    break;
  case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    // Advertising data setup complete
    adv_data_complete_cb(param, h);
    break;
  case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
    // Scan response data setup complete
    adv_rsp_complete_cb(param, h);
    break;
  case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    adv_start_cb(param, h);
    break;
  case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
    // Update connection parameters
    conn_params_update_cb(param, h);
  default:
    //TODO
    break;
  }
  return;
}

// Scan parameters complete callback.
void scan_param_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  if(param->scan_param_cmpl.status==ESP_BT_STATUS_SUCCESS) {
    ESP_LOGV(TAG, "Scan parameter setup complete.");
    h->scan_params_complete=1;
  }
  return;
}
// Start scan callback
void scan_start_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  // check if scanning was succesful
  if(param->scan_start_cmpl.status==ESP_BT_STATUS_SUCCESS) {
    ESP_LOGI(TAG, "Scan started.");
  } else {
    ESP_LOGE(TAG, "Unable to start scan process.");
    //TODO(Emanuele): Fix the could not scan bug, for now loop back.
    h->core_cb_args->scan.status=0; // Scan failed.
    if(h->core_cb!=NULL) {
      (*h->core_cb)(ON_SCAN_END, h->core_cb_args);
    }
  }
  return;
}

/*
 * TODO
 * check validity of the entry (eg. check payload to confirm bemesh protocol
 * can be initialized).
 */
static int entry_valid(esp_ble_gap_cb_param_t* param) {
  uint8_t* adv_data=param->scan_rst.ble_adv;
  uint8_t adv_data_len=param->scan_rst.adv_data_len;
  // Search for GAP_SRV_ER. After that byte we should find the advertised srv uuid
  for(int i=0;i<adv_data_len;++i) {
    if(adv_data[i]==GAP_SRV_ER) {
      int valid=true;
      uint8_t* srv_data=adv_data+i+1;
      // Check the current possible srv_data payload.
      // if it contains the bemesh_gap_service_uuid then
      // the entry is valid
      for(int j=0;j<GAP_SRV_UUID_LEN;++j) {
	if(i+j >= adv_data_len) {
	  // buffer exceeded
	  valid=false;
	  break;
	}
	if(srv_data[j] != bemesh_gap_service_uuid[j]) {
	  // wrong element in the srv_data
	  valid=false;
	  break;
	}
      }
      if(valid==true) {
	return valid;
      }
    }
  }
  return false;
}
/*
 * Auxilary function to check and eventually store the new scanned dev into
 * found_devs_vect
 * Also update found_devs var.
 */
static int insert_entry(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  // check each entry in the found_devs_vect.
  // if the bda is found, do not insert
  if(h->found_devs==GAP_HANDLER_SCAN_DEVS_MAX) {
    // If the list is already full, no insertion is possible.
    return 0;
  }
  // Check that the enty is a bemesh node
  
  if(!entry_valid(param)) {
    return 0;
  }
  uint8_t *new_bda=param->scan_rst.bda;
  for(int i=0;i<h->found_devs;++i) {
    uint8_t* stored_bda=h->found_devs_vect[i].bda;
    uint8_t found=true;
    // check if the mac corresponds.
    for(int j=0;j<ESP_BD_ADDR_LEN;j++) {
      if(stored_bda[j]!=new_bda[j]) {
	found=false;
	break;
      }
    }
    // if corrispondency is found, stop the insertion.
    if(found) {
      return 0;
    }
  }
  // The new_bda was never seen before, can insert.
  // Increase found_devs.
  bemesh_dev_t* new_dev=&h->found_devs_vect[h->found_devs++];
  // Fill the new device struct.
  memcpy(new_dev->bda, param->scan_rst.bda, ESP_BD_ADDR_LEN);
  memcpy(&new_dev->rssi, &param->scan_rst.rssi, sizeof(int));
  return 1;
}

/*
 * Scan result callback. 2 cases:
 * -new device found (ESP_GAP_SEARCH_INQ_RES_EVT)
 * -scan terminated (ESP_GAP_SEARCH_INQ_CMPL_EVT)
 */
static void print_scanned_dev_info(esp_ble_gap_cb_param_t* param) {
  char buf[64];
  int wb=sprintf(buf, "Found new device with mac: ");
  for(int i=0;i<ESP_BD_ADDR_LEN; ++i) {
    wb+=sprintf(buf+wb, "%02X.", param->scan_rst.bda[i]);
  }
  sprintf(buf+wb, " with rssi: %d ", param->scan_rst.rssi);
  ESP_LOGI(TAG, "%s", buf);
  return;
}

static void scan_result_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  if(param->scan_rst.search_evt==ESP_GAP_SEARCH_INQ_RES_EVT) {
    // A new device is found. Check that it was not found previously.
    // And eventually store it in found_devs_vect
    int ret=insert_entry(param, h);
    // If it was not found previously, print the infos
    if(ret) {
      print_scanned_dev_info(param);
    }
  } else if(param->scan_rst.search_evt==ESP_GAP_SEARCH_INQ_CMPL_EVT) {
    // set scan complete flag
    h->flags|=O_SCNCMPL;
    // clear scanning flag.
    h->flags&=~O_SCN;
    ESP_LOGI(TAG, "Scan procedure terminated.");
    // Launch core callback handler
    // setup the params
    h->core_cb_args->scan.result=h->found_devs_vect;
    h->core_cb_args->scan.len=h->found_devs;
    h->core_cb_args->scan.status=1; // Scan was succesful.
    // Launch ON_SCAN_END event to core.
    if(h->core_cb!=NULL) {
      (*h->core_cb)(ON_SCAN_END, h->core_cb_args);
    }
  }
  return;
}

// Advertisement data passing proc. complete
static void adv_data_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  ESP_LOGI(TAG, "Advertising data setup complete.");
  h->adv_data_set=true;
  return;
}

// Scan response data passing proc. complete
static void adv_rsp_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  ESP_LOGI(TAG, "Scan response data setup complete.");
  h->rsp_data_set=true;
  return;
}

// Advertisement proc. start callback
static void adv_start_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  if(param->adv_start_cmpl.status==ESP_BT_STATUS_SUCCESS) {
    ESP_LOGI(TAG, "Advertising started.");
  } else {
    ESP_LOGE(TAG, "Unable to start advertising.");
  }
  return;
}

// Updated connection parameters (Event caused by the gatts or gattc handlers)
static void conn_params_update_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  ESP_LOGI(TAG, "Updated connection params: status=%d, min_int=%d, max_int=%d, conn_int=%d, latency=%d, timeout=%d",
	   param->update_conn_params.status,
	   param->update_conn_params.min_int,
	   param->update_conn_params.max_int,
	   param->update_conn_params.conn_int,
	   param->update_conn_params.latency,
	   param->update_conn_params.timeout);
  //ESP_LOGI(TAG, "Starting new advertisement proc.");
  //esp_ble_gap_start_advertising(&h->adv_params);
  return;
}
