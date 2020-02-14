/*
 * gap_handler.c
 * Handler for GAP operations
 */

#include "gap_handler.h"
#include "esp_log.h" // Logging library
#include "nvs_flash.h"
#include <string.h> // memcpy

static const char* TAG = "gap_handler";

// Since we want one and only one gap_handler we will statically define it.
static bemesh_gap_handler gap1;
static bemesh_gap_handler *get_gap1_ptr(void) {
  return &gap1;
}

// GAP callback handler definition
void bemesh_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

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
  adv->include_name=true; // Do not include the name in the resp packet.

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
static void setup_scanning_params(esp_ble_scan_params_t* params,
				  bemesh_gap_handler* h) {
  params->scan_type=BLE_SCAN_TYPE_ACTIVE;
  params->own_addr_type=BLE_ADDR_TYPE_PUBLIC;
  params->scan_filter_policy=BLE_SCAN_FILTER_ALLOW_ALL;
  params->scan_interval=0x50;
  params->scan_window=0x30;
  return;
}

// Initializes the ble structures in the gap handler h.
bemesh_gap_handler* bemesh_gap_handler_init(uint8_t* rsp_buffer,
					    uint8_t rsp_buffer_len,
					    uint8_t *srv_uuid_buffer,
					    uint8_t srv_uuid_len) {
  // Initial routine to activate bluedroid stack (PLEASE REMOVE AFTER ENABLING CORE LIB)
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
  esp_bt_controller_config_t bt_cfg=BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
  esp_bluedroid_init();
  esp_bluedroid_enable();

  // set callback function
  ESP_ERROR_CHECK(esp_ble_gap_register_callback(bemesh_gap_cb));
  
  // get gap1 ptr
  bemesh_gap_handler* h=get_gap1_ptr();
  // Setup the response manufacturer buffer
  h->rsp_man_buffer=rsp_buffer;
  h->rsp_man_buffer_len=rsp_buffer_len;
  // Setup the structures of gap handler
  setup_advertising_data(&h->adv_data, h, srv_uuid_buffer, srv_uuid_len); // adv data
  setup_resp_data(&h->rsp_data, h); // rsp data
  setup_advetising_params(&h->adv_params, h); // adv params
  setup_scanning_params(&h->scan_params, h); // scan params

  // initialize the handler's vars
  h->scan_params_complete=0;
  h->found_devs=0;
  h->adv_data_set=false;
  h->rsp_data_set=false;

  // Execute paramter passing routines to esp
  esp_ble_gap_set_device_name("ESP32BLEMESH");
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

// CENTRAL MODE callbacks (scan)
void scan_param_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
void scan_start_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
void scan_result_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);

// PERIPHERAL MODE callbacks (adv)
void adv_data_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
void adv_rsp_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);
void adv_start_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h);

/*
 * GAP Event handler.
 * Executed when a new GAP event occurs.
 */
void bemesh_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
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
  }
  return;
}


/*
 * check validity of the entry (eg. check payload to confirm bemesh protocol
 * can be initialized).
 */
static int entry_valid(esp_ble_gap_cb_param_t* param);
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
  sprintf(buf+wb, " with rssi: %d", param->scan_rst.rssi);
  ESP_LOGI(TAG, "%s", buf);
  return;
}

void scan_result_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  if(param->scan_rst.search_evt==ESP_GAP_SEARCH_INQ_RES_EVT) {
    // A new device is found. Check that it was not found previously.
    // And eventually store it in found_devs_vect
    int ret=insert_entry(param, h);
    // If it was not found previously, print the infos
    if(ret) {
      print_scanned_dev_info(param);
    }
  } else if(param->scan_rst.search_evt==ESP_GAP_SEARCH_INQ_CMPL_EVT) {
    ESP_LOGI(TAG, "Scan procedure terminated.");
  }
  return;
}

// Advertisement data passing proc. complete
void adv_data_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  ESP_LOGI(TAG, "Advertising data setup complete.");
  h->adv_data_set=true;
  return;
}

// Scan response data passing proc. complete
void adv_rsp_complete_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  ESP_LOGI(TAG, "Scan response data setup complete.");
  h->rsp_data_set=true;
  return;
}

// Advertisement proc. start callback
void adv_start_cb(esp_ble_gap_cb_param_t* param, bemesh_gap_handler* h) {
  if(param->adv_start_cmpl.status==ESP_BT_STATUS_SUCCESS) {
    ESP_LOGI(TAG, "Advertising started.");
  } else {
    ESP_LOGE(TAG, "Unable to start advertising.");
  }
  return;
}
