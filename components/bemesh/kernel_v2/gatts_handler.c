/*
 * gatts_handler.h
 * Handler for GATT-Server related operations
 */

#include "esp_log.h"
#include "gatts_handler.h"
#include <string.h>

static const char* TAG = "gatts_handler";

// Since we want one and only one bemesh_core_t we will statically define it.
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
  h->char1_val.attr_max_len=0;
  h->char1_val.attr_value=gatts_char_buffer;
  return;
}

bemesh_gatts_handler *bemesh_gatts_handler_init(void) {
  bemesh_gatts_handler *h=get_gatts1_ptr();

  // Install the gatts handler
  esp_ble_gatts_register_callback(bemesh_gatts_cb);
  // Initialize the characteristic.
  gatts_char_init(h);
  
  // Initialize the gatts profile, then install it.
  gatts_profile_init(&h->profile_inst);  
  esp_ble_gatts_app_register(GATTS_APP_ID); 
  
  return h;
}
/*
void bemesh_gatts_handler_install_cb(bemesh_gatts_handler* h, bemesh_gatts_cb cb, void* cb_args) {
  if(cb!=NULL && h->ext_gatts_handler_cb==NULL) {
    h->ext_gatts_handler_cb=cb;
    h->ext_gatts_handler_cb_args=cb_args;
  }
  return;
}

void bemesh_gatts_handler_remove_cb(bemesh_gatts_handler* h) {
  h->ext_gatts_handler_cb=NULL;
  h->ext_gatts_handler_cb_args=NULL;
  return;
}
*/
// Event callbacks.
static void app_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void serv_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void char_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void char_descr_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void connection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void disconnection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void read_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
static void write_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h);
//TODO

/*
 * GATTS Event handler.
 * Executed when a new GATTS event occurs.
 */
void bemesh_gatts_cb(esp_gatts_cb_event_t event,
		     esp_gatt_if_t gatts_if,
		     esp_ble_gatts_cb_param_t *param) {
  bemesh_gatts_handler *h=get_gatts1_ptr();
  switch(event) {
  case ESP_GATTS_REG_EVT:
    // New Application is registered. (First event)
    app_reg_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_CREATE_EVT:
    // New service is registered.
    serv_reg_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_ADD_CHAR_EVT:
    // New characteristic is registered.
    char_reg_cb(gatts_if, param, h);
    break;
  case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    // New characteristid descriptor registered.
    char_descr_reg_cb(gatts_if, param, h);
    break;
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
    //TODO
  default:
    //TODO
    ESP_LOGI(TAG, "Recived unhandled event no. %d", event);
    break;
  }
  /*
  if(h->ext_gatts_handler_cb!=NULL) {
    (*h->ext_gatts_handler_cb)(event, gatts_if, param, h, h->ext_gatts_handler_cb_args);
  }
  */
  return;
}

// Application registered callback
static void app_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  // Store the gatts_if in the application struct
  if(param->reg.status==ESP_GATT_OK) {
    h->profile_inst.gatts_if=gatts_if;
    ESP_LOGI(TAG, "New application registered...Registering primary service.");
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
  esp_gatt_char_prop_t att_prop=ESP_GATT_CHAR_PROP_BIT_READ |
    ESP_GATT_CHAR_PROP_BIT_WRITE |
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

// Characteristic registered callback
// Setup the profile_inst struct with the given handles from the BLE stack
static void char_reg_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  // Setup the char handle
  h->profile_inst.char_handle=param->add_char.attr_handle;
  h->profile_inst.descr_uuid.len=ESP_UUID_LEN_16;
  h->profile_inst.descr_uuid.uuid.uuid16=ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
  // Testing the reading procedure for the attribute.
  uint16_t attr_len=0;
  const uint8_t *read_attr;
  esp_err_t ret=esp_ble_gatts_get_attr_value(param->add_char.attr_handle,
						      &attr_len,
						      &read_attr);
  if(ret==ESP_FAIL) {
    ESP_LOGE(TAG, "Error: Illegal characteristic handle.");
  }
  ESP_LOGV(TAG, "Current characteristic length: %x", attr_len);
  for(int i=0;i<attr_len;++i) {
    ESP_LOGV(TAG, "read_attr[%x] = %x", i, read_attr[i]);
  }

  // Adding characteristic descriptor
  ret=esp_ble_gatts_add_char_descr(h->profile_inst.service_handle,
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
  ESP_LOGI(TAG, "Succesfully installed characteristic descr: status %d, attr_handle %d, service_handle %d",
	   param->add_char.status, param->add_char.attr_handle,
	   param->add_char.service_handle);
  return;
}

// New incoming connection callback
static void connection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  // Generate a connection params struct for the new connection
  esp_ble_conn_update_params_t conn_params={0};
  memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t)); // store bda
  conn_params.latency=0; 
  conn_params.max_int=0x30; // max_int = 0x30*1.25ms = 40ms
  conn_params.min_int=0x10; // min_int = 0x10*1.25ms = 20ms
  conn_params.timeout=400;  // timeout = 400*10ms  = 4000ms
  ESP_LOGI(TAG, "Received new connection (%d) from %02x.%02x.%02x.%02x.%02x.%02x",
	   param->connect.conn_id, param->connect.remote_bda[0], param->connect.remote_bda[1],
	   param->connect.remote_bda[2], param->connect.remote_bda[3], param->connect.remote_bda[4],
	   param->connect.remote_bda[5]);
  // update the profile connection id
  h->profile_inst.conn_id=param->connect.conn_id;
  // Update the connection parameters to the peer device.
  esp_ble_gap_update_conn_params(&conn_params);
  return;
}

static void disconnection_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Disconnected event.");
  // Launch the gatts callback handler for higher order operations
  return;
}

// Receive a reading request, therefore we must send back a response with the requested payload
static void read_cb(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  ESP_LOGI(TAG, "Received read event from %d, trans_id:%d, handle:%d",
	   param->read.conn_id, param->read.trans_id, param->read.handle);
  // Construct the response
  esp_gatt_rsp_t rsp;
  memset(&rsp, 0, sizeof(rsp));
  rsp.attr_value.handle=param->read.handle;
  rsp.attr_value.len=4;
  rsp.attr_value.value[0]=0xde;
  rsp.attr_value.value[1]=0xad;
  rsp.attr_value.value[2]=0xbe;
  rsp.attr_value.value[3]=0xb0;
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
  // Print some debug info.
  ESP_LOGI(TAG, "Received write characteristic operation: len:%d, value:", param->write.len);
  esp_log_buffer_hex(TAG, param->write.value, param->write.len);
  // check that the correct descr is chosen
  if(h->profile_inst.descr_handle==param->write.handle &&
     param->write.len==2) {
    // Read the two 
    uint16_t descr_value=param->write.value[1]<<8|param->write.value[0];
    
  }
     
  return;
}

static void _write_long_characteristic(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, bemesh_gatts_handler* h) {
  /*
   * Used for write long characteristic operation ( > one ATT MTU)
   */
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
