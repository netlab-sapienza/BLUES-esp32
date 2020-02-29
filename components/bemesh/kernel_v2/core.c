/*
 * core.c
 * Handles operations directly with the ESP module.
 */


#include <string.h>  // memcpy

#include "include/core.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt_device.h" // esp_bt_dev_get_address


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// logging tag
static const char* TAG = "core";

// Since we want one and only one bemesh_core_t we will statically define it.
static bemesh_core_t core1;
static bemesh_core_t *get_core1_ptr(void) {
  return &core1;
}

// Handler for low level handlers.
// This callback should relaunch the higher level callbacks

static void low_handlers_cb(bemesh_kernel_evt_t event,
                            bemesh_evt_params_t* params);


void core_peripheral_init(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
  esp_bluedroid_init();
  esp_bluedroid_enable();
}

static void log_own_bda(void) {
  char buf[64];
  int wb = sprintf(buf, "Initializing device's core with BDA: ");
  const uint8_t* bda = bemesh_core_get_bda(get_core1_ptr());
  for (int i = 0; i < ESP_BD_ADDR_LEN; ++i) {
    wb += sprintf(buf+wb, "%02X.", bda[i]);
  }
  ESP_LOGI(TAG, "%s", buf);
  return;
}

bemesh_core_t* bemesh_core_init(void) {
  // SET LOGGING LEVEL TO WARNING
  //esp_log_level_set(TAG, ESP_LOG_WARN);
  
  core_peripheral_init();
  bemesh_core_t *core=get_core1_ptr();

  // Setup core vars
  core->outgoing_conn_len=0;
  core->incoming_conn_len=0;
  for (int i = 0; i < KERNEL_EVT_NUM; ++i) {
    core->handler_cb[i]=NULL;
  }
  for (int i = 0; i < GATTC_MAX_CONNECTIONS; ++i) {
    core->outgoing_conn[i].conn_id=CORE_UNUSED_CONN_ID;
  }
  for (int i = 0; i < GATTS_MAX_CONNECTIONS; ++i) {
    core->incoming_conn[i].conn_id=CORE_UNUSED_CONN_ID;
  }
    
  // Link the gatts_handler to core1 and initialize it
  core->gattsh=bemesh_gatts_handler_init();
  bemesh_gatts_handler_install_cb(core->gattsh, low_handlers_cb, &core->handler_cb_args);
  // Link the gattc_handler to core1 and initialize it
  core->gattch=bemesh_gattc_handler_init();
  bemesh_gattc_handler_install_cb(core->gattch, low_handlers_cb, &core->handler_cb_args);
  // Link the gap_handler to core1 and initialize it
  core->gaph=bemesh_gap_handler_init(NULL, 0, NULL, 0); // TODO: Add rsp and srv_uuid buffers
  bemesh_gap_handler_install_cb(core->gaph, low_handlers_cb, &core->handler_cb_args);
  // Print device BDA.
  log_own_bda();
  //TODO
  return core;
}

// Returns the current dev bda. TODO: increase descr.
uint8_t *bemesh_core_get_bda(bemesh_core_t* c) {
  return esp_bt_dev_get_address();
}

/* GAP HANDLING */
// Scanning ops
int bemesh_core_start_scanning(bemesh_core_t* c, uint16_t timeout) {
  // start the scan proc.
  return bemesh_gap_handler_start_scanning(c->gaph, timeout);
}

int bemesh_core_stop_scanning(bemesh_core_t* c) {
  // stop the scan proc.
  bemesh_gap_handler_stop_scanning(c->gaph);
  return 0;
}
uint8_t bemesh_core_scan_complete(bemesh_core_t* c) {
  // returns the scan complete status flag of gaph
  return bemesh_gap_handler_scan_complete(c->gaph);
}
uint8_t bemesh_core_is_scanning(bemesh_core_t* c) {
  // returns the status of the scanning proc.
  return (c->gaph->flags&O_SCN);
}
/* returns the scan result array length.
 * bemesh_core_scan_complete function should be called first
 * to know if scan procedure is complete.
 */
uint8_t bemesh_core_get_scan_result_len(bemesh_core_t* c) {
  return bemesh_gap_handler_get_scan_res_len(c->gaph);
}
/* Returns a pointer to an array containing the scan results.
 * Refer to bemesh_dev_t definition to 
 */
bemesh_dev_t *bemesh_core_get_scan_result(bemesh_core_t* c) {
  return bemesh_gap_handler_get_scan_res(c->gaph);
}
// Advertising ops
int bemesh_core_start_advertising(bemesh_core_t* c) {
  return bemesh_gap_handler_start_advertising(c->gaph);
}
// TODO(Andrea): Add descr
int bemesh_core_stop_advertising(bemesh_core_t* c) {
  bemesh_gap_handler_stop_advertising(c->gaph);
  return 0;
}
// TODO(Andrea): Add descr
uint8_t bemesh_core_is_advertising(bemesh_core_t* c) {
  return (c->gaph->flags&O_ADV);
}

/* GATT HANDLING */
/* establish a connection with a remote dev that has bda bda
 */
int bemesh_core_connect(bemesh_core_t* c, esp_bd_addr_t bda) {
  // Obfuscate gatts connection event handler
  ESP_LOGV(TAG, "Obfuscating gatts");
  c->gattsh->flags|=O_IGNCONN;
  int ret = bemesh_gattc_open(c->gattch, bda, 0);
  return ret;
}
/* disconnects from a remote dev that has bda bda
 * TODO: Add descr
 */
int bemesh_core_disconnect(bemesh_core_t* c, esp_bd_addr_t bda);
// TODO(Andrea): Add descr
int bemesh_core_write(bemesh_core_t* c,
                      uint16_t conn_id,
                      uint8_t *src, uint16_t len, uint8_t is_notify) {
  if (is_notify) {
    // Must use notify system for communication.
    bemesh_gatts_handler_send_notify(c->gattsh,
                                     conn_id,
                                     src,
                                     len);
  } else {
    // Can rely on standard write proc.
    bemesh_gattc_handler_write(c->gattch,
                               conn_id,
                               src, len,
                               true);
  }
  return 0;
}
// TODO(Andrea): Add descr
int bemesh_core_read(bemesh_core_t* c, uint16_t conn_id,
                     uint8_t *dest, uint16_t len) {
  return 0;
  // TODO(Emanuele): Complete the function.
}

// Install the handler for kernel events
void bemesh_core_install_callback(bemesh_core_t *c,
                                  bemesh_kernel_evt_t evt, kernel_int_cb cb) {
  if (evt > KERNEL_EVT_NUM) {
    ESP_LOGW(TAG, "Warning: cannot install a callback for an unvalid event.");
    return;
  }
  c->handler_cb[evt] = cb;
  return;
}
// Uninstall the handler for kernel events
void bemesh_core_uninstall_callback(bemesh_core_t *c, bemesh_kernel_evt_t evt) {
  if (evt > KERNEL_EVT_NUM) {
    ESP_LOGW(TAG, "Warning: cannot uninstall a callback for an unvalid event.");
    return;
  }
  c->handler_cb[evt] = NULL;
  return;
}

/*
 * Insert a new entry in the dest array
 */
static int insert_conn_entry(bda_id_tuple* dest, uint8_t* bda, uint16_t conn_id, uint8_t len) {
  for(int i=0;i<len;++i) {
    bda_id_tuple *entry=&dest[i];
    if(entry->conn_id==CORE_UNUSED_CONN_ID) {
      // Store the entry here
      memcpy(entry->bda, bda, ESP_BD_ADDR_LEN);
      entry->conn_id=conn_id;
      return 0;
    }
  }
  return 1;
}
/*
 * Remove an entry from the dest array
 */
static int remove_conn_entry(bda_id_tuple* dest, uint8_t* bda, uint8_t len) {
  for(int i=0;i<len;++i) {
    bda_id_tuple *entry=&dest[i];
    uint8_t *entry_bda=entry->bda;
    uint8_t found=true;
    for(int j=0;j<ESP_BD_ADDR_LEN;++j) {
      if(entry_bda[j]!=bda[j]) {
	found=false;
	break;
      }
    }
    if(found) {
      // If the bda is found, clear the entry.
      memset(entry_bda, 0, ESP_BD_ADDR_LEN);
      entry->conn_id=CORE_UNUSED_CONN_ID;
      return 0;
    }
  }
  return 1;
}




struct task_handler_params_t {
  kernel_int_cb cb;
  bemesh_evt_params_t *args;
};

static void task_handler_cb(void *task_params) {
  struct task_handler_params_t *args=(struct task_handler_params_t *)task_params;
  ESP_LOGI(TAG, "Launching task_handler_cb.");
  // execute the callback
  (*args->cb)(args->args);
  // Free the allocated structure.
  free(task_params);
  // Destroy the task.
  ESP_LOGI(TAG, "Destroying task_handler_cb.");
  vTaskDelete(NULL);
}
// Handler for low level handlers. This callback should relaunch the higher level callbacks
static void low_handlers_cb(bemesh_kernel_evt_t event, bemesh_evt_params_t *params) {
  bemesh_core_t *c=get_core1_ptr();
  int ret;
  switch(event) {
  case ON_SCAN_END:
    ESP_LOGI(TAG, "ON_SCAN_END event");
    break;
  case ON_MSG_RECV:
    ESP_LOGI(TAG, "ON_MSG_RECV event, len %d",
	     params->recv.len);
    break;
  case ON_INC_CONN:
    ESP_LOGI(TAG, "ON_INC_CONN event");
    if(params->conn.ack) {
      // Store the new connection parameters in the incoming_conn buffer.
      ESP_LOGI(TAG, "Preparing to insert new connection entry.");
      ret=insert_conn_entry(c->incoming_conn, (uint8_t*)params->conn.remote_bda,
			    params->conn.conn_id, GATTS_MAX_CONNECTIONS);
      ESP_LOGI(TAG, "Done.");
      if(ret) {
	ESP_LOGE(TAG, "Error: could not insert the new entry in the core database.");
      } else {
	++c->incoming_conn_len; // Increase the current number of connections.
      }
    }    
    break;
  case ON_OUT_CONN:
    ESP_LOGI(TAG, "ON_OUT_CONN event");
    if(params->conn.ack) {
      // Store the new connection parameters in the incoming_conn buffer.
      ret=insert_conn_entry(c->outgoing_conn, (uint8_t*)params->conn.remote_bda,
			    params->conn.conn_id, GATTC_MAX_CONNECTIONS);
      if(ret) {
	ESP_LOGE(TAG, "Error: could not insert the new entry in the core database.");
      } else {
	++c->outgoing_conn_len; // Increase the current number of connections.
      }
    }    
    break;
  case ON_DISCONN:
    ESP_LOGI(TAG, "ON_DISCONN event");
    // In theory the same bda cannot live both in outgoing_conn and incoming_conn
    // hence try to remove the entry on both arrays.
    remove_conn_entry(c->outgoing_conn, (uint8_t *)params->conn.remote_bda, GATTC_MAX_CONNECTIONS);
    remove_conn_entry(c->incoming_conn, (uint8_t *)params->conn.remote_bda, GATTS_MAX_CONNECTIONS);
    break;
  case ON_READ_REQ:
    ESP_LOGI(TAG, "ON_READ_REQ event");
    break;
  }
  // After that, launch the higher level callback.
  if(c->handler_cb[event]!=NULL) {
    struct task_handler_params_t* task_args=(struct task_handler_params_t *)malloc(sizeof(struct task_handler_params_t));
    task_args->cb=c->handler_cb[event];
    task_args->args=&c->handler_cb_args;
    // Launch the task
    xTaskCreate(&task_handler_cb, "core_cb_handler", 4086, task_args, tskIDLE_PRIORITY, NULL);
    //(*c->handler_cb[event])(params);
  }
  return;
}

/**
 *  Compares bda1 address with bda2 address.
 *  @param bda1 first address
 *  @param bda2 second address
 *  @return 1 if bda1 equals bda2, 0 otherwise
 */ 
int bda_equals(esp_bd_addr_t bda1, esp_bd_addr_t bda2) {
  int eq=true;
  for(int i=0;i<ESP_BD_ADDR_LEN;++i) {
    if(bda1[i]!=bda2[i]) {
      eq=false;
      break;
    }
  }
  return eq;
}
