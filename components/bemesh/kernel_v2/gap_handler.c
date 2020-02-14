/*
 * gap_handler.c
 * Handler for GAP operations
 */

#include "gap_handler.h"

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
				   bemesh_gap_handler* h) {
  adv->set_scan_rsp=false; // This is not the response structure.
  adv->include_name=false; // Include the name in the adv packet.
  adv->min_interval=0x06; // Minimum advertising interval : 0x06 * 1.25ms = 7.5 ms.
  adv->max_interval=0x10; // Maximum advertising interval : 0x10 * 1.25ms = 20 ms.
  adv->appearance=0x00; // standard appearance value

  adv->manufacturer_len=0;
  adv->p_manufacturer_data=NULL;

  adv->service_data_len=0;
  adv->p_service_data=NULL;
  // TODO: Setup the service uuid payload.
  adv->service_uuid_len=0;
  adv->p_service_uuid=NULL;

  adv->flag = (ESP_BLE_ADV_FLAG_GEN_DISC |
	       ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
  return;
}

// Configure the response data for the module.
static void setup_resp_data(esp_ble_adv_data_t* adv,
			    bemesh_gap_handler* h) {
  adv->set_scan_rsp=true; // This is the response structure.
  adv->include_name=true; // Include the name in the adv packet.

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
int bemesh_gap_handler_init(bemesh_gap_handler* h,
			    uint8_t rsp_buffer,
			    uint8_t rsp_buffer_len) {
  // Setup the response manufacturer buffer
  h->rsp_man_buffer=rsp_buffer;
  h->rsp_man_buffer_len=rsp_buffer_len;
  // Setup the structures of gap handler
  setup_advertising_data(&h->adv_data, h); // adv data
  setup_resp_data(&h->rsp_data, h); // rsp data
  setup_advetising_params(&h->adv_params, h); // adv params
  setup_scanning_params(&h->scan_params, h); // scan params
  
  return 0;
}

// Configure the operational mode on the gap handler h.
void bemesh_gap_handler_mode(bemesh_gap_handler* h, uint8_t m) {
  return;
}

