/*
 * gattc_handler.c
 * Handler for GATT-Client related operations
 */

#include "gattc_handler.h"
#include "esp_log.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "gattc_handler";

// Since we want one and only one bemesh_gattc_handler we will statically define it.
static bemesh_gattc_handler gattc1;
static bemesh_gattc_handler *get_gattc1_ptr(void) {
  return &gattc1;
}

// GATTC callback handler definition
static void bemesh_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
			    esp_ble_gattc_cb_param_t *param);

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

// Extract the correct application profile, given a conn_id
static gattc_profile_inst *__get_gattc_profile_connid(uint16_t conn_id,
						      gattc_profile_inst *p_vect) {
  for(uint8_t i=0;i<GATTC_APP_PROFILE_INST_LEN;++i) {
    if(p_vect[i].conn_id==conn_id && p_vect[i].gattc_if!=ESP_GATT_IF_NONE) {
      return &p_vect[i];
    }
  }
  return NULL;
}

static void profile_inst_vect_init(gattc_profile_inst* p) {
  for(int i=0;i<GATTC_APP_PROFILE_INST_LEN;++i) {
    p[i].gattc_if=ESP_GATT_IF_NONE;
  }
}

// We look for the bemesh digital service provided by the server
static void remote_filter_serv_uuid_init(esp_bt_uuid_t *r) {
  r->len=ESP_UUID_LEN_16;
  r->uuid.uuid16=GATTS_SERV_UUID;
}

// We look for the bemesh characteristic through this filter
static void remote_filter_char_uuid_init(esp_bt_uuid_t *r) {
  r->len=ESP_UUID_LEN_16;
  r->uuid.uuid16=GATTS_CHAR_UUID;
}

bemesh_gattc_handler *bemesh_gattc_handler_init(void) {
  bemesh_gattc_handler *h=get_gattc1_ptr();
  // Initialize the app profile vect.
  profile_inst_vect_init(h->profile_inst_vect);

  // Set gattc_handler vars
  h->server_valid_flag=false;
  remote_filter_serv_uuid_init(&h->remote_filter_service_uuid);
  remote_filter_char_uuid_init(&h->remote_filter_char_uuid);

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
  char *bemesh_gattc_open_tag="[gattc_open]";
  esp_err_t ret=ESP_FAIL;
  for(int i=0;i<GATTC_APP_PROFILE_INST_LEN;++i) {
    // If the current app profile is not bounded to any remote, use that to connect.
    ESP_LOGV(bemesh_gattc_open_tag, "(%d) -> gattc_if:%d", i, h->profile_inst_vect[i].gattc_if);
    if(h->profile_inst_vect[i].gattc_if!=ESP_GATT_IF_NONE) {
      // Open the connection with the remote bda.
      ESP_LOGI(TAG, "Opening connection with gattc_if: %d", h->profile_inst_vect[i].gattc_if);
      ret=esp_ble_gattc_open(h->profile_inst_vect[i].gattc_if,
			     remote_bda, remote_addr_type, true);
      if(ret!=ESP_OK) {
	ESP_LOGW(TAG, "Warning: something went wrong during gattc_open, errcode=%d", ret);
      }
      return ret;
    }
  }
  ESP_LOGW(TAG, "Warning: no available app_profiles ready to host the connection.");
  return ret;
}

void bemesh_gattc_handler_install_cb(bemesh_gattc_handler *h, kernel_cb cb, bemesh_evt_params_t *params) {
  h->core_cb=cb;
  h->core_cb_args=params;
}
void bemesh_gattc_handler_uninstall_cb(bemesh_gattc_handler *h) {
  h->core_cb=NULL;
  h->core_cb_args=NULL;
}

//TODO descr
void bemesh_gattc_handler_write(bemesh_gattc_handler *h, uint16_t conn_id,
				uint8_t *data, uint16_t data_len, uint8_t resp) {
  gattc_profile_inst *prof=__get_gattc_profile_connid(conn_id, h->profile_inst_vect);
  if(!prof) {
    ESP_LOGE(TAG, "Error: no profile applications contains conn_id %d", conn_id);
    return;
  }
  uint16_t gattc_if=prof->gattc_if;
  uint16_t char_handle=prof->char_handle;
  esp_err_t ret=esp_ble_gattc_write_char(gattc_if,
					 conn_id,
					 char_handle,
					 data_len,
					 data,
					 ESP_GATT_WRITE_TYPE_RSP,
					 ESP_GATT_AUTH_REQ_NONE);
  if(ret!=ESP_GATT_OK) {
    ESP_LOGW(TAG, "Warning: could not write characteristic, errcode=%d", ret);
  }
}

// Event callbacks.
static void app_reg_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void connection_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void copen_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void cfg_mtu_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void search_serv_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void search_serv_cmpl_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void write_chr_cmpl_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void reg_notify_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);
static void recv_notify_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h);




// GATTC callback handler definition
static void bemesh_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
			    esp_ble_gattc_cb_param_t *param) {
  bemesh_gattc_handler *h=get_gattc1_ptr();
  ESP_LOGI(TAG, "Received EVENT %02X, gattc_if: %02X", event, gattc_if);
  switch(event) {
  case ESP_GATTC_REG_EVT:
    // New application is registered. (First event)
    app_reg_cb(gattc_if, param, h);
    break;
    /* Not handling connection event as it creates collisions with connect event in gatts */
    //case ESP_GATTC_CONNECT_EVT:
    // Connection established with a new server
    //connection_cb(gattc_if, param, h);
    //  break;
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
  case ESP_GATTC_SEARCH_CMPL_EVT:
    // Service search complete.
    search_serv_cmpl_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_WRITE_CHAR_EVT:
    // Write characteristic complete.
    write_chr_cmpl_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_REG_FOR_NOTIFY_EVT:
    // Register to notify complete.
    reg_notify_cb(gattc_if, param, h);
    break;
  case ESP_GATTC_WRITE_DESCR_EVT:
    // Write descriptor complete.
    if(param->write.status!=ESP_GATT_OK) {
      ESP_LOGE(TAG, "Error: could not write on the descriptor, errcode=%d",
	       param->write.status);
    }
    break;
  case ESP_GATTC_NOTIFY_EVT:
     // Receive notify. */
     ESP_LOGE(TAG, "ESP_GATTC_NOTIFY_EVT received.");
     recv_notify_cb(gattc_if, param, h);
    break;
  default:
    ESP_LOGW(TAG, "Warning: received unhandled evt %d", event);
    //TODO
    break;
  }
}

static void app_reg_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->reg.status==ESP_GATT_OK) {
    h->profile_inst_vect[param->reg.app_id].gattc_if=gattc_if;					       
    ESP_LOGI(TAG, "New application registered (gattc_if:%02x, stored:%02x at app:%02x", gattc_if,
	     h->profile_inst_vect[param->reg.app_id].gattc_if,
	     param->reg.app_id);
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
}

// Connection opened callback
static void copen_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->open.status!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: open failed, errcode=%X", param->open.status);
  }
  ESP_LOGI(TAG, "Open operation succesful. gattc_if:%d, conn_id:%d", gattc_if, param->open.conn_id);
  // Get the correct app profile.
  gattc_profile_inst *profile_inst=__get_gattc_profile(gattc_if, h->profile_inst_vect);
  // Copy the remote BDA
  profile_inst->conn_id=param->open.conn_id;
  memcpy(profile_inst->remote_bda, param->open.remote_bda, sizeof(esp_bd_addr_t));
  ESP_LOG_BUFFER_HEX(TAG,profile_inst->remote_bda,ESP_BD_ADDR_LEN);
  ESP_LOG_BUFFER_HEX(TAG,param->open.remote_bda,ESP_BD_ADDR_LEN);
  ESP_LOGI(TAG,"%d",(int) sizeof(esp_bd_addr_t));
  // Send local MTU to server
  esp_err_t ret=esp_ble_gattc_send_mtu_req(gattc_if, param->open.conn_id);
  if(ret) {
    ESP_LOGE(TAG, "Error: could not config MTU, errcode=%X", ret);
  }
  // Execute core handler callback
  if(h->core_cb!=NULL) {
    // Fill the params struct.
    memcpy(h->core_cb_args->conn.remote_bda, param->open.remote_bda, sizeof(esp_bd_addr_t));
    h->core_cb_args->conn.conn_id=profile_inst->conn_id;
    // KEEP IN MIND THAT CONNECTION IS STILL NOT SECURED. The ON_OUT_CONN event may be launched
    // after the service discovery complete event.
    //(*h->core_cb)(ON_OUT_CONN, h->core_cb_args);
  }
}

// MTU config done callback
static void cfg_mtu_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->cfg_mtu.status!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: could not configure MTU properly, errcode=%d", param->cfg_mtu.status);
  }
  ESP_LOGI(TAG, "Configured MTU with size: %d", param->cfg_mtu.mtu);
  ESP_LOGI(TAG, "Starting search-service routine.");
  // Search for the bemesh service
  esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &h->remote_filter_service_uuid);
}

// Search service callback
static void search_serv_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  esp_gatt_srvc_id_t *srvc_id=&param->search_res.srvc_id;
  ESP_LOGI(TAG, "Search res: conn_id:%d, is_primary:%d", param->search_res.conn_id,
	   param->search_res.is_primary);
   
  // Check if we found the service that we're looking for.
  if(srvc_id->id.uuid.len==h->remote_filter_service_uuid.len &&
     srvc_id->id.uuid.uuid.uuid16==h->remote_filter_service_uuid.uuid.uuid16) {
    // Validity of the server confirmed.
    h->server_valid_flag=true;
    gattc_profile_inst* prof=__get_gattc_profile(gattc_if, h->profile_inst_vect);
    // Setup start and end handle, used to get characteristics
    prof->service_start_handle=param->search_res.start_handle;
    prof->service_end_handle=param->search_res.end_handle;
    ESP_LOGI(TAG, "Found UUID16: %X", srvc_id->id.uuid.uuid.uuid16);
  }
  }

static void search_serv_cmpl_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->search_cmpl.status!=ESP_GATT_OK) {
    ESP_LOGW(TAG, "Warning: could not complete service search succesfully, errcode=%d",
	     param->search_cmpl.status);
    return;
  }
  ESP_LOGI(TAG, "Service search complete. Getting characteristic value.");
  uint16_t conn_id=param->search_cmpl.conn_id;
  gattc_profile_inst* prof=__get_gattc_profile(gattc_if, h->profile_inst_vect);
  // Get the attribute counts from the local db.
  uint16_t att_count;
  esp_gatt_status_t ret=esp_ble_gattc_get_attr_count(gattc_if,
						     conn_id,
						     ESP_GATT_DB_CHARACTERISTIC,
						     prof->service_start_handle,
						     prof->service_end_handle,
						     ESP_GATT_INVALID_HANDLE,
						     &att_count);
  if(ret) {
    ESP_LOGE(TAG, "Error: could not get attribute count: errcode=%d", ret);
  }

  if(att_count>0) {
    esp_gattc_char_elem_t* char_elem_res=(esp_gattc_char_elem_t*)malloc(sizeof(esp_gattc_char_elem_t)* att_count);
    if(!char_elem_res) {
      ESP_LOGE(TAG, "Could not allocate space for char_elem_res.");
    } else {
      // Search for the characteristic whose uuid is signed in h->remote_filter_char_uuid
      ret=esp_ble_gattc_get_char_by_uuid(gattc_if,
					 conn_id,
					 prof->service_start_handle,
					 prof->service_end_handle,
					 h->remote_filter_char_uuid,
					 char_elem_res,
					 &att_count);
      if(ret!=ESP_GATT_OK) {
	ESP_LOGE(TAG, "Error: could not get characteristics by uuid, errcode=%d", ret);
      }
      if(att_count>0) {
	// We found the correct characteristic.
	// In every case, we'll pick the first selection (even if only one exists)
	ESP_LOGI(TAG, "Found the bemesh characteristic with UUID16: %04X", char_elem_res[0].uuid.uuid.uuid16);
	//TODO: register for notifies.
	// Check for notify propriety
	if(char_elem_res[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) {
	  ESP_LOGI(TAG, "Requesting notify to the characteristic.");
	  prof->char_handle=char_elem_res[0].char_handle;
 	  esp_err_t reg_notify_ret=esp_ble_gattc_register_for_notify(gattc_if, prof->remote_bda,
								     char_elem_res[0].char_handle);
 	  ESP_LOG_BUFFER_HEX(TAG,prof->remote_bda,ESP_BD_ADDR_LEN);
	  if(reg_notify_ret!=ESP_OK) {
            ESP_LOGE(TAG, "Error: Could not register for notifies, errcode=%d",
                     reg_notify_ret);
          }else{
	    ESP_LOGI(TAG,"Registered notify");
	  }
	}
      }
      free(char_elem_res);
    }   
  }
  }

static void write_chr_cmpl_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  ESP_LOGI(TAG, "Write characteristic complete.");
}

static esp_bt_uuid_t notify_descr_uuid = {
  .len=ESP_UUID_LEN_16,
  .uuid={.uuid16=ESP_GATT_UUID_CHAR_CLIENT_CONFIG},
};

static void reg_notify_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->reg_for_notify.status!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: could not register for notify, status=%d",
	     param->reg_for_notify.status);
    return;
  }
  gattc_profile_inst* prof=__get_gattc_profile(gattc_if, h->profile_inst_vect);
  uint16_t conn_id=prof->conn_id;
  uint16_t notify_enable=1; // Enable notifies (not indicates.)
  //uint16_t notify_enable=2; // Enable indicates (not notifies.)
  uint16_t count=0; // number of attributes found.
  ESP_LOGI(TAG, "Initializing notify registering proc. gattc_if:%d, conn_id:%d",
	   gattc_if, conn_id);
  esp_gatt_status_t ret=esp_ble_gattc_get_attr_count(gattc_if,
						     conn_id,
						     ESP_GATT_DB_DESCRIPTOR,
						     prof->service_start_handle,
						     prof->service_end_handle,
						     prof->char_handle,
						     &count);
  if(ret!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: could not execute get_attr_count, errcode=%d", ret);
  }
  if(count>0) {
    esp_gattc_descr_elem_t *descr_elem_res=malloc(sizeof(esp_gattc_descr_elem_t)*count);
    if(!descr_elem_res) {
      ESP_LOGE(TAG, "Error: malloc error");
    }
    ret=esp_ble_gattc_get_descr_by_char_handle(gattc_if,
					       conn_id,
					       param->reg_for_notify.handle,
					       notify_descr_uuid,
					       descr_elem_res,
					       &count);
    if(ret!=ESP_GATT_OK) {
      ESP_LOGE(TAG, "esp_ble_gattc_get_descr_by_char_handle failed.");
    }
    if(count>0 && descr_elem_res[0].uuid.len==ESP_UUID_LEN_16 &&
       descr_elem_res[0].uuid.uuid.uuid16==ESP_GATT_UUID_CHAR_CLIENT_CONFIG) {
      // Write on the client config descriptor.
      ret=esp_ble_gattc_write_char_descr(gattc_if,
					 conn_id,
					 descr_elem_res[0].handle,
					 sizeof(notify_enable),
					 (uint8_t*)&notify_enable,
					 ESP_GATT_WRITE_TYPE_RSP,
					 ESP_GATT_AUTH_REQ_NONE);
      if(ret!=ESP_GATT_OK) {
	ESP_LOGE(TAG, "Error: could not write in the client config descr.");
      } else {
	ESP_LOGI(TAG, "Write to client config descriptor with success.");
      }
    }
    free(descr_elem_res);
  } else {
    ESP_LOGW(TAG, "Warning, could not find any attrs");
  }
  // Launch the ON_OUT_CONN event to core.
  if(h->core_cb!=NULL) {
    // Please refer to copen_cb for cb_args constructions.
    (*h->core_cb)(ON_OUT_CONN, h->core_cb_args);
  }
  }

static void recv_notify_cb(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param, bemesh_gattc_handler* h) {
  if(param->notify.is_notify) {
    ESP_LOGI(TAG, "Received notify.");
  } else {
    ESP_LOGI(TAG, "Received indicate.");
  }
  esp_log_buffer_hex(TAG, param->notify.value, param->notify.value_len);
}

