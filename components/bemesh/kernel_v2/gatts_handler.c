/*
 * gatts_handler.h
 * Handler for GATT-Server related operations
 */

#include "esp_log.h"
#include "gatts_handler.h"
#include "esp_err.h"
#include <string.h>

static const char* TAG = "gatts_handler";

// Since we want one and only one bemesh_gatts_handler we will statically define it.
static bemesh_gatts_handler gatts1;
static bemesh_gatts_handler *get_gatts1_ptr(void) {
  return &gatts1;
}

// GATTS callback handler definition
void bemesh_gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


/*
 * Initialize the service struct inside the app profile.
 * We are going to setup a primary service with 4 handles.
 * -Service handle
 * -Characteristic handle
 * -Characteristic value handle
 * -Characteristic descriptor handle
 */
static void gatts_service_init(gatts_profile_inst* p) {
  p->service_id.is_primary=true; // set as primary serv.
  p->service_id.id.inst_id=0x00; // set serv. instance as 0
  // Set UUID for generic access service.
  p->service_id.id.uuid.len=ESP_UUID_LEN_16;
  p->service_id.id.uuid.uuid.uuid16=GATTS_SERV_UUID;
}
// Initialize the GATTS Profile struct.
static void gatts_profile_init(gatts_profile_inst* p) {
  p->gatts_if=ESP_GATT_IF_NONE;
  gatts_service_init(p);
}

// Initialize the characteristic of the gatts_handler
// We store incoming data into a static buffer.
// On successive actions, data will be rapidly memcpy'd
// on other buffers.
static uint8_t gatts_char_buffer[GATT_CHAR_BUF_SIZE];
static void gatts_char_init(bemesh_gatts_handler *h) {
  h->char1_val.attr_max_len=GATT_CHAR_BUF_SIZE;
  // We set the length of the attribute to 4, or else the esp api's will ragequit :(
  h->char1_val.attr_len=4;
  h->char1_val.attr_value=gatts_char_buffer;

  // Write some random crap inside the buffer
  gatts_char_buffer[0]=0xDE;
  gatts_char_buffer[1]=0xAD;
  gatts_char_buffer[2]=0xBE;
  gatts_char_buffer[3]=0xB0;
  return;
}

bemesh_gatts_handler *bemesh_gatts_handler_init(void) {
  // SET LOGGING LEVEL TO WARNING
  esp_log_level_set(TAG, ESP_LOG_WARN);
  
  bemesh_gatts_handler *h=get_gatts1_ptr();

  // Install the gatts handler
  esp_ble_gatts_register_callback(bemesh_gatts_cb);
  // Initialize the characteristic.
  gatts_char_init(h);
  
  // Initialize the gatts profile, then install it.
  gatts_profile_init(&h->profile_inst);  
  esp_ble_gatts_app_register(GATTS_APP_ID);

  // Set the MTU size (Used GATTC mtu size for comodity).
  esp_ble_gatt_set_local_mtu(GATTC_MTU_SIZE);
  return h;
}

void bemesh_gatts_handler_install_cb(bemesh_gatts_handler *h, kernel_cb cb, bemesh_evt_params_t *params) {
  h->core_cb=cb;
  h->core_cb_args=params;
  return;
}
void bemesh_gatts_handler_uninstall_cb(bemesh_gatts_handler *h) {
  h->core_cb=NULL;
  h->core_cb_args=NULL;
  return;
}

//TODO: add descr
void bemesh_gatts_handler_send_notify(bemesh_gatts_handler *h,
				      uint16_t conn_id,
				      uint8_t *data,
				      uint16_t data_len) {
  // get the gatts_if
  uint16_t gatts_if=h->profile_inst.gatts_if;
  uint16_t char_handle=h->profile_inst.char_handle;
  ESP_LOGI(TAG, "Preparing notify with params: gatts_if:%d, conn_id:%d, char_handle:%d",
	   gatts_if, conn_id, char_handle);
  esp_err_t ret=esp_ble_gatts_send_indicate(gatts_if,
					    conn_id,
					    char_handle,
					    data_len,
					    data,
					    false); // false for notification, true for indicate
  if(ret!=ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: could not send indicate, errcode=%d", ret);
  }
  return;
}

// Event callbacks.
static void app_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void serv_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void serv_cmpl_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void char_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void char_descr_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void connection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void disconnection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void read_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void write_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void exec_write_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void recv_conf_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
//TODO

/*
 * GATTS Event handler.
 * Executed when a new GATTS event occurs.
 */
void bemesh_gatts_cb(esp_gatts_cb_event_t event,
		     esp_gatt_if_t gatts_if,
		     esp_ble_gatts_cb_param_t *param) {
  bemesh_gatts_handler *h=get_gatts1_ptr();
  //ESP_LOGI(TAG, "Received EVENT %02X, gatts_if: %02X", event, gatts_if);
  switch(event) {
  case ESP_GATTS_REG_EVT:
    // New Application is registered. (First event)
    app_reg_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_CREATE_EVT:
    // New service is registered.
    serv_reg_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_START_EVT:
    // Start service complete
    serv_cmpl_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_ADD_CHAR_EVT:
    // New characteristic is registered.
    char_reg_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    // New characteristid descriptor registered.
    char_descr_reg_cb(gatts_if, param, h);
    break;
    /* Not handling connection event as it creates collisions with gattc*/
  case ESP_GATTS_CONNECT_EVT:
    // A new client connects to the server.
    connection_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_DISCONNECT_EVT:
    disconnection_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_READ_EVT:
    // Receive read request
    read_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_WRITE_EVT:
    write_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_EXEC_WRITE_EVT:
    exec_write_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_CONF_EVT:
    recv_conf_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_SET_ATTR_VAL_EVT:
    ESP_LOGV(TAG, "Attribute set.");
    break;
  case ESP_GATTS_RESPONSE_EVT:
    ESP_LOGV(TAG, "Send response.");
    break;
  case ESP_GATTS_MTU_EVT:
    ESP_LOGV(TAG, "MTU config complete.");
    break;
    //TODO
  default:
    //TODO
    ESP_LOGW(TAG, "Warning: received unhandled evt %d", event);
    break;
  }
  return;
}

// Application registered callback
static void app_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received ESP_GATTS_REG_EVT");
  // Store the gatts_if in the application struct
  if(param->reg.status==ESP_GATT_OK) {
    h->profile_inst.gatts_if=gatts_if;
    ESP_LOGI(TAG, "New application registered (gatts_if:%02x)...Registering primary service.",
	     gatts_if);
    // Creating the primary service.
    esp_ble_gatts_create_service(gatts_if, &h->profile_inst.service_id, GATTS_NUM_HANDLES);
  } else {
    ESP_LOGW(TAG, "Warning: Could not register new application.");
  }
  return;
}

// Service registered callback
// Construct the new characteristic and install it.
static void serv_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received ESP_GATTS_CREATE_EVT");
  // store in the profile_inst the service_handle generated by the BLE stack.
  h->profile_inst.service_handle=param->create.service_handle;
  // Setup the characteristic uuid
  h->profile_inst.char_uuid.len=ESP_UUID_LEN_16;
  h->profile_inst.char_uuid.uuid.uuid16=GATTS_CHAR_UUID;

  // start the service
  esp_ble_gatts_start_service(h->profile_inst.service_handle);

  // Define the characteristic proprieties:
  // Allow read
  // Allow write
  // Allowed to notify changes
  esp_gatt_char_prop_t att_prop=0;
  att_prop=ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE |
    ESP_GATT_CHAR_PROP_BIT_NOTIFY;
  esp_err_t add_char_ret;
  // Create a new characteristic, linked to h->char_buffer buffer on which
  // new data will be stored.
  add_char_ret=esp_ble_gatts_add_char(h->profile_inst.service_handle,
				      &h->profile_inst.char_uuid,
				      ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
				      att_prop,
				      &h->char1_val,
				      NULL);
  //errcheck
  if(add_char_ret) {
    ESP_LOGE(TAG, "Error: could not add new characteristic, errcode =%x", add_char_ret);
  } else {
    ESP_LOGI(TAG, "Service registration succesful.");
  }
  return;
}

// Start service complete callback.
static void serv_cmpl_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received ESP_GATTS_START_EVT");
  if(param->start.status==ESP_GATT_OK) {
    ESP_LOGI(TAG, "Service start complete succesful with service_handle: %d", param->start.service_handle);  
  } else {
    ESP_LOGW(TAG, "Warning: could not start the service, gatt_status: %02X",
	     param->start.status);
  }
  return; 
}

// Characteristic registered callback
// Setup the profile_inst struct with the given handles from the BLE stack
static void char_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received ESP_GATTS_ADD_CHAR_EVT");
  // Setup the char handle
  h->profile_inst.char_handle=param->add_char.attr_handle;
  h->profile_inst.descr_uuid.len=ESP_UUID_LEN_16;
  h->profile_inst.descr_uuid.uuid.uuid16=ESP_GATT_UUID_CHAR_CLIENT_CONFIG;  
  // Adding characteristic descriptor
  esp_gatt_status_t ret=esp_ble_gatts_add_char_descr(h->profile_inst.service_handle,
						     &h->profile_inst.descr_uuid,
						     ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
						     NULL, NULL);
  if(ret) {
    ESP_LOGE(TAG, "Error: could not add characteristic descriptor, errcode =%x", ret);
  } else {
    ESP_LOGI(TAG, "Characteristic descriptor registration succesful.");
  }
  
  return;
}

// Characteristic descriptor registered callback
static void char_descr_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received ESP_GATTS_ADD_CHAR_DESCR_EVT");
  ESP_LOGI(TAG, "Succesfully installed characteristic descr: status %d, attr_handle %d, service_handle %d",
	   param->add_char.status, param->add_char.attr_handle,
	   param->add_char.service_handle);
  // store the descriptor handle
  h->profile_inst.descr_handle=param->add_char_descr.attr_handle;
  return;
}

// New incoming connection callback
static void connection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  if(h->flags&O_IGNCONN) {
    ESP_LOGV(TAG, "Discarding connection evt. Disabling IGNCONN flag.");
    // Re-enabling the connection handling
    h->flags&=~O_IGNCONN;
    return;
  }
  // Generate a connection params struct for the new connection
  esp_ble_conn_update_params_t conn_params={0};
  memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t)); // store bda
  conn_params.latency=0; 
  conn_params.max_int=0x30; // max_int = 0x30*1.25ms = 40ms
  conn_params.min_int=0x10; // min_int = 0x10*1.25ms = 20ms
  conn_params.timeout=400;  // timeout = 400*10ms  = 4000ms
  ESP_LOGI(TAG, "Received new connection conn_id:%d, gatts_if:%d, from %02x.%02x.%02x.%02x.%02x.%02x",
	   param->connect.conn_id, gatts_if, param->connect.remote_bda[0], param->connect.remote_bda[1],
	   param->connect.remote_bda[2], param->connect.remote_bda[3], param->connect.remote_bda[4],
	   param->connect.remote_bda[5]);
  // update the profile connection id
  h->profile_inst.conn_id=param->connect.conn_id;
  // Update the connection parameters to the peer device.  
  esp_ble_gap_update_conn_params(&conn_params);

  // Execute core handler callback
  if(h->core_cb!=NULL) {
    // Fill the params struct.
    memcpy(h->core_cb_args->conn.remote_bda, param->connect.remote_bda, ESP_BD_ADDR_LEN);
    h->core_cb_args->conn.conn_id=param->connect.conn_id;
    h->core_cb_args->conn.ack=true;
    (*h->core_cb)(ON_INC_CONN, h->core_cb_args);
  }
  return;
}

static void disconnection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGV(TAG, "Disconnected event.");
  // Execute core handler callback
  if(h->core_cb!=NULL) {
    // Fill the params struct.
    memcpy(h->core_cb_args->conn.remote_bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
    (*h->core_cb)(ON_DISCONN, h->core_cb_args);
  }
  return;
}

// Receive a reading request, therefore we must send back a response with the requested payload
static void read_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received read event from %d, trans_id:%d, handle:%d",
	   param->read.conn_id, param->read.trans_id, param->read.handle);
  // Construct the response
  esp_gatt_rsp_t rsp;
  memset(&rsp, 0, sizeof(rsp));
  /* rsp.attr_value.handle=param->read.handle; */
  /* rsp.attr_value.len=h->char1_val.attr_len; */
  /* memcpy(rsp.attr_value.value, */
  /* 	 h->char1_val.attr_value, */
  /* 	 h->char1_val.attr_len); */
  uint8_t* char_value;
  esp_ble_gatts_get_attr_value(h->profile_inst.char_handle,
			       &rsp.attr_value.len,
			       (const uint8_t**)&char_value);
  memcpy(rsp.attr_value.value, char_value, rsp.attr_value.len);
  esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
			      param->read.trans_id,
			      ESP_GATT_OK, &rsp);
  return;
}

/*
 * Receive a write request. Bear in mind that there are two types of write operations:
 * -write characteristic (for payloads <= 23b, or one ATT MTU)
 * -write long characteristic ( >= 23b )
 * We will implement both.
 */

static void _write_characteristic(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  /*
   * Used for write characteristic operation (one ATT MTU)
   */
  // Check if the write request is related to the client config descriptor.
  uint8_t client_config_flag=false;
  if(h->profile_inst.descr_handle==param->write.handle&&
     param->write.len==2) {
    client_config_flag=true;
    // Check the data sent by the peer
    uint16_t descr_value=(param->write.value[1]<<8) | param->write.value[0];
    if(descr_value==0x01) {
      ESP_LOGI(TAG, "Notify enabled.");
    } else if(descr_value==0x02) {
      ESP_LOGI(TAG, "Indication enabled.");
    } else if(descr_value==0x00) {
      ESP_LOGI(TAG, "Notify/indications disabled.");
    } else {
      ESP_LOGW(TAG, "Unknown char descr value.");
    }
  }
  // Print some debug info.
  //ESP_LOGI(TAG, "Received write characteristic operation: len:%d, value:", param->write.len);
  //esp_log_buffer_hex(TAG, param->write.value, param->write.len);
  // Check offset for the write
  if(param->write.need_rsp) {
    // Generate a response.
    esp_err_t ret=esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
					      param->write.trans_id, ESP_GATT_OK,
					      NULL);
    if(ret!=ESP_OK) {
      ESP_LOGE(TAG, "Error: send response error %d", ret);
      return;
    }
  }

  // We don't need to go further if the write was relative to the client configuration
  if(client_config_flag) {
    return;
  }
  
  ESP_LOGV(TAG, "Received write char op. len:%d", param->write.len);
  if(param->write.len>GATT_CHAR_BUF_SIZE) {
    ESP_LOGW(TAG, "Warning: received payload is too long.");
    return;
  }
  // Copy the stuff on the characteristic.
  //memcpy(h->char1_val.attr_value, param->write.value, param->write.len);
  //h->char1_val.attr_len=param->write.len;
  // Try to use the ble stack function instead of memcpy
  esp_ble_gatts_set_attr_value(h->profile_inst.char_handle,
			       param->write.len,
			       param->write.value);
  // TEST: Send a notification back to the client
  /* esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, */
  /* 			      h->profile_inst.char_handle, */
  /* 			      param->write.len, param->write.value, false); */
  
  // Execute core handler callback
  if(h->core_cb!=NULL) {
    // Fill the params struct.
    memcpy(h->core_cb_args->recv.remote_bda, param->write.bda, sizeof(esp_bd_addr_t));
    uint16_t payload_len;
    uint8_t *payload_ptr;
    esp_ble_gatts_get_attr_value(h->profile_inst.char_handle,
				 &payload_len,
				 (const uint8_t**)&payload_ptr);
    // Temporary solution since get_attr wont return the new received payload in the payload_ptr.
    // We will memcpy on the payload_ptr and pass that as parameter.
    memcpy(payload_ptr, param->write.value, param->write.len);
    payload_len = param->write.len;
    /* h->core_cb_args->recv.payload=param->write.value; */
    /* h->core_cb_args->recv.len=param->write.len; */
    h->core_cb_args->recv.payload=payload_ptr;
    h->core_cb_args->recv.len=payload_len;
    (*h->core_cb)(ON_MSG_RECV, h->core_cb_args);
  }  
  return;
}

static void _write_long_characteristic(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  /*
   * Used for write long characteristic operation ( > one ATT MTU)
   */
  ESP_LOGI(TAG, "Executing long write routine");
  esp_gatt_status_t status=ESP_GATT_OK;
  // Check if the sub-payload offset is in the buffer limits
  if(param->write.offset>EXEC_WRITE_BUF_LEN) {
    status=ESP_GATT_INVALID_OFFSET;
  } else if(param->write.offset+param->write.len>EXEC_WRITE_BUF_LEN) {
    status=ESP_GATT_INVALID_ATTR_LEN;
  }
  if(param->write.need_rsp) {
    esp_gatt_rsp_t *gatt_rsp=&h->gatt_rsp;
    gatt_rsp->attr_value.len=param->write.len;
    gatt_rsp->attr_value.handle=param->write.handle;
    gatt_rsp->attr_value.offset=param->write.offset;
    gatt_rsp->attr_value.auth_req=ESP_GATT_AUTH_REQ_NONE;
    memcpy(gatt_rsp->attr_value.value,
	   param->write.value,
	   param->write.len);
    esp_err_t ret=esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
					      param->write.trans_id, status,
					      gatt_rsp);
    if(ret!=ESP_OK) {
      ESP_LOGE(TAG, "Error: send response error %d", ret);
      return;
    }
  }
  // Copy the data in the characteristic.
  memcpy(h->exec_write_buffer+param->write.offset, param->write.value, param->write.len);
  h->exec_write_len+=param->write.len;
  return;
}

static void write_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  if(!param->write.is_prep) {
    // is_prep is set true only if write long char. operation is going on
    _write_characteristic(gatts_if, param, h);
  } else {
    _write_long_characteristic(gatts_if, param, h);
  }
  return;
}

// End long write sequence callback
static void exec_write_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received end execute write event.");
  esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK,
			      NULL);
  // Copy the payload in the characteristic.
  esp_ble_gatts_set_attr_value(h->profile_inst.char_handle,
			       h->exec_write_len,
			       h->exec_write_buffer);
  // Execute core handler callback
  if(h->core_cb!=NULL) {
    // Fill the params struct.
    memcpy(h->core_cb_args->recv.remote_bda, param->write.bda, sizeof(esp_bd_addr_t));
    uint16_t payload_len;
    uint8_t *payload_ptr;
    esp_ble_gatts_get_attr_value(h->profile_inst.char_handle,
				 &payload_len,
				 (const uint8_t**)&payload_ptr);
    h->core_cb_args->recv.payload=payload_ptr;
    h->core_cb_args->recv.len=h->exec_write_len;
    (*h->core_cb)(ON_MSG_RECV, h->core_cb_args);
  }
  // Reset the exec_write variable for future long writes
  h->exec_write_len=0;
  return;
}

static void recv_conf_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  if(param->conf.status != ESP_GATT_OK) {
    ESP_LOGE(TAG, "Error: Could not send indicate, errcod=%d", param->conf.status);
  } else {
    //ESP_LOGI(TAG, "Notify confirm received with success.");
  }
  return;
}
