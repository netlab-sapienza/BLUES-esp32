/*
 * gattc_handler.c
 * Handler for GATT-Client related operations
 */

#include "gattc_handler.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "gattc_handler";

// Since we want one and only one bemesh_gattc_handler we will statically define it.
static bemesh_gattc_handler gattc1;
static bemesh_gattc_handler *get_gattc1_ptr(void) {
  return &gattc1;
}

// GATTC callback handler definition
static void bemesh_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
			    esp_ble_gattc_cb_param_t *param);

static void profile_inst_vect_init(gattc_profile_inst* p) {
  for(int i=0;i<GATTC_APP_PROFILE_INST_LEN;++i) {
    p[i].gattc_if=ESP_GATT_IF_NONE;
  }
  return;
}

// We look for the bemesh digital service provided by the server
static void remote_filter_serv_uuid_init(esp_bt_uuid_t *r) {
  r->len=ESP_UUID_LEN_16;
  r->uuid.uuid16=GATTS_SERV_UUID;
}

bemesh_gattc_handler *bemesh_gattc_handler_init(void) {
  bemesh_gattc_handler *h=get_gattc1_ptr();
  // Initialize the app profile vect.
  profile_inst_vect_init(h->profile_inst_vect);

  // Set gattc_handler vars
  h->server_valid_flag=false;

  // register the callback function for the gattc module
  esp_ble_gattc_register_callback(bemesh_gattc_cb);
  
  // Set the MTU size
  esp_ble_gatt_set_local_mtu(GATTC_MTU_SIZE);

  // Install the app profile.
  esp_ble_gattc_app_register(GATTC_APP_ID);  
  return h;  
}

// Open connection with a remote device. Returns -1 if no free gatt intefaces are available
uint8_t bemesh_gattc_open(bemesh_gattc_handler* h, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type) {
  esp_err_t ret=ESP_FAIL;
  for(int i=0;i<GATTC_APP_PROFILE_INST_LEN;++i) {
    // If the current app profile is not bounded to any remote, use that to connect.
    if(h->profile_inst_vect[i].conn_id==0 && h->profile_inst_vect[i].gattc_if!=ESP_GATT_IF_NONE) {
      // Open the connection with the remote bda.
      ESP_LOGI(TAG, "Opening connection with gattc_if: %d", h->profile_inst_vect[i].gattc_if);
      ret=esp_ble_gattc_open(h->profile_inst_vect[i].gattc_if,
			     remote_bda, remote_addr_type, true);
      return ret;
    }
  }
  return ret;
}

// Event callbacks.
static void app_reg_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void connection_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void copen_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void cfg_mtu_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void search_serv_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);

// Extract the correct application profile, given a gattc_if
static gattc_profile_inst *__get_gattc_profile(esp_gatt_if_t gattc_if, gattc_profile_inst* p_vect) {
  // Assume that gattc_if is present in one of the app profiles in p_vect
  for(uint8_t i=0;i<GATTC_APP_PROFILE_INST_LEN;++i) {
    if(p_vect[i].gattc_if==gattc_if) {
      return &p_vect[i];
    }
  }
  return NULL;
}

// GATTC callback handler definition
static void bemesh_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
			    esp_ble_gattc_cb_param_t *param) {
  bemesh_gattc_handler *h=get_gattc1_ptr();
  switch(event) {
  case ESP_GATTC_REG_EVT:
    // New application is registered. (First event)
    app_reg_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_CONNECT_EVT:
    // Connection established with a new server
    connection_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_OPEN_EVT:
    // Connection opened with a server
    copen_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_CFG_MTU_EVT:
    // Configuration of MTU 
    cfg_mtu_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_SEARCH_RES_EVT:
    // Service search result
    search_serv_cb(gattc_if, param, h);
    break;
  default:
    //TODO
    break;
  }
  return;
} 
static void app_reg_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->reg.status==ESP_GATT_OK) {
    h->profile_inst_vect[param->reg.app_id].gattc_if=gattc_if;
    ESP_LOGI(TAG, "New application registered.");
  } else {
    ESP_LOGW(TAG, "Warning: Could not register new application.");
  }
}

// Connection happening callback
static void connection_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  ESP_LOGI(TAG, "Connection event: conn_id: %d, if %d",
	   param->connect.conn_id, gattc_if);
  // Get the correct app profile.
  gattc_profile_inst *profile_inst=__get_gattc_profile(gattc_if, h->profile_inst_vect);
  // Copy the remote BDA
  profile_inst->conn_id=param->connect.conn_id;
  memcpy(profile_inst->remote_bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
  // Send local MTU to server
  esp_err_t ret=esp_ble_gattc_send_mtu_req(gattc_if, param->connect.conn_id);
  if(ret) {
    ESP_LOGE(TAG, "Error: could not config MTU, errcode=%X", ret);
  }

  // Before going forward on copen_cb, reset the server validity flag
  h->server_valid_flag=false;
  return;
}

// Connection opened callback
static void copen_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->open.status!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: open failed, errcode=%X", param->open.status);
  }
  ESP_LOGI(TAG, "Open operation succesful.");
  return;
}

// MTU config done callback
static void cfg_mtu_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->cfg_mtu.status!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: could not configure MTU properly, errcode=%d", param->cfg_mtu.status);
  }
  ESP_LOGI(TAG, "Configured MTU with size: %d", param->cfg_mtu.mtu);
  ESP_LOGI(TAG, "Starting search-service routine.");
  esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &h->remote_filter_service_uuid);
  return;
}

// Search service callback
static void search_serv_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  esp_gatt_srvc_id_t *srvc_id=&param->search_res.srvc_id;
  ESP_LOGI(TAG, "Search res: conn_id:%d, is_primary:%d", param->search_res.conn_id, param->search_res.is_primary);

   
  // Check if we found the service that we're looking for.
  if(srvc_id->id.uuid.len==h->remote_filter_service_uuid.len &&
     srvc_id->id.uuid.uuid.uuid16==h->remote_filter_service_uuid.uuid.uuid16) {
    // Validity of the server confirmed.
    h->server_valid_flag=true;
    gattc_profile_inst* prof=__get_gattc_profile(gattc_if, h->profile_inst_vect);
    // Setup start and end handle, used to get characteristics
    prof->service_start_handle=param->search_res.start_handle;
    prof->service_end_handle=param->search_res.end_handle;
    ESP_LOGI(TAG, "Found UUID16: %d", srvc_id->id.uuid.uuid.uuid16);
  }
  return;
}

