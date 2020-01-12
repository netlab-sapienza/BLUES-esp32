/*
 *  	LIBRARIES
 */

#pragma once
#include "gatts_table.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"

#include "esp_gatts_api.h"
#include "esp_bt_defs.h"

#include <stdlib.h>
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "constant.hpp"
#include <stdio.h>

#define GATTS_CHAR_VAL_LEN_MAX 255 //was 0x40
#define MAC_LEN 6
#define SCAN_LIMIT 20 // Scan is limited up to 100 different devices

#define TOTAL_NUMBER_LIMIT 7 // Total of incoming and outgoing edges is 7
#define CLIENTS_NUMBER_LIMIT 4 // Incoming links of clients
#define SERVERS_NUMBER_LIMIT 3 // Outgoing or incoming connections with servers
#define PROFILE_NUM      1
// Macros for the ID_TABLE

#define CLIENT 0
#define SERVER 1
#define GATTC_TAG "GATT_CLIENT"
#define GATTS_TAG "GATT_SERVER"
#define REMOTE_SERVICE_UUID        0x00FF
#define REMOTE_NOTIFY_CHAR_UUID    0xFF01
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0


#define SERVERS_NUM 3
#define SERVER_S1 0
#define SERVER_S2 1
#define SERVER_S3 2
#define INVALID_HANDLE   0

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
} ;


struct device {
	uint8_t* dev_name; // Name of the device
	uint8_t* mac; // Mac address of the device
	uint8_t addr_type; // BLE addr type
	uint8_t clients_num; // Number of clients connected to that device
	uint8_t rssi; // Received signal strength indication
};

extern uint8_t CHR_VALUES[HRS_IDX_NB][GATTS_CHAR_VAL_LEN_MAX];
extern uint16_t CHR_HANDLES[HRS_IDX_NB];
extern struct gattc_profile_inst gl_profile_tab2[PROFILE_NUM];
extern uint8_t MACS[TOTAL_NUMBER_LIMIT][MAC_ADDRESS_SIZE];
extern struct device scan_res[SCAN_LIMIT];


extern bool becoming_client;
extern bool becoming_server;

extern bool wants_to_discover;
extern bool wants_to_send_routing_table;
extern bool conn_device_S1;
extern bool conn_device_S2;
extern bool conn_device_S3;

extern bool get_service_S1;
extern bool get_service_S2;
extern bool get_service_S3;


typedef void(*NotifyCb)(uint16_t,uint8_t,uint8_t);
typedef void(*InitCb)(uint8_t);
typedef void(*ShutDownCb)(uint8_t);
//This callback function pass the newly updated MAC table entry to the master object.
typedef void(*ServerUpdateCb)(uint8_t*,uint8_t,uint16_t,uint8_t,uint8_t);
//This callback function is triggered whenever two servers meet for the first time so that they
//can exchange their routing tables.
typedef void(*ExchangeRoutingTableCb)(uint8_t*,uint8_t*,uint16_t,uint8_t);
typedef void (*SendRoutingTableCb)(uint8_t*,uint8_t*,uint16_t,uint8_t,uint8_t);
typedef void(*ReceivedPacketCb)(uint8_t* packet,uint16_t len);

typedef void(*EndScanning)(struct device* list, uint8_t scan_seq,uint8_t type); // Returns details of nearby devices
typedef void(*ServerLost)();

/****** SERVER - SERVER COMMUNICATION [SSC]
 * 	ACTIVE: internal_client connects to a server: can perform a write_chr / read_chr
 *  PASSIVE: server has a new internal_client connected: can only perform a notification
*******/

typedef void(*SSC_Active)(uint8_t internal_client_id); // internal_client_id is the client calling the cb, can be SERVER_S1 / S2 / S3
typedef void(*SSC_Passive)(uint8_t conn_id); // conn_id is the connection id given by the server to the new internal_client


//
/*
 *  	FUNCTIONS DECLARATION
 */

// Scanning functions
void processDevice(esp_ble_gap_cb_param_t *scan_result, uint8_t *adv_name, uint8_t adv_len); // Add or update the device in the array of scanned devices
uint8_t connectTo(struct device dev,uint8_t flag_internal, uint8_t num_internal_client); // Establish a connection with dev and returns 1 if an error eccurs, 0 otherwise. flag internal is 1 if an internal_client is calling it => set num_internal client. otherwise both parameters are 0.
// connectTo can be used in a server (as internal_client) with the internal_client number. Otherwise leave it to 0.
void scan(uint8_t duration, uint8_t num_internal_client); // Start scanning with duration in seconds. Eventually add internal_client or leave it to 0.


// Mutation functions
void becomeServer(); // If no servers were found during the scanning let the client become a server.


// Internal clients for a server
void esp_gattc_internal_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
void esp_gap_S1_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void esp_gap_S2_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void esp_gap_S3_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void gattc_profile_S1_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
void gattc_profile_S2_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
void gattc_profile_S3_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

// Client
void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

// Server
void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// Main functions
void ble_esp_startup(); // Use this to start the entire project
void register_internal_client(uint8_t client_num); // Registration of an internal client for a server
void unregister_internal_client(uint8_t client_num);
void gatt_client_main(); // Registration of an ESP client
void gatt_server_main(); // Registration of an ESP server
void unregister_client();
void unregister_server();
void start_internal_client(uint8_t client); // internal clients are SERVER_S1, SERVER_S2, SERVER_S3. This includes the registration
void change_name(uint8_t flag, uint8_t idx); // Flag: 1 -> +, 0 -> -

uint8_t notify_client(uint8_t conn_id, uint8_t chr, uint8_t* data, uint8_t data_size); // Send notification to a client (conn_id) using a characteristic channel


//Trasferite in characteristic.h

uint8_t find_CHR(uint16_t handle); // Given an handle find the characteristic it refers to
uint8_t write_CHR(uint16_t gattc_if, uint16_t conn_id, uint8_t chr, uint8_t* array, uint8_t len); // Returns 1 if an error occurs, 0 otherwise
uint8_t* read_CHR(uint16_t gattc_if, uint16_t conn_id, uint8_t chr);
uint8_t get_CHR_value_len(uint8_t chr); // Get the lenght of the last read value of a characteristic


// SERVER / CLIENT FUNCTIONS
uint8_t get_num_connections(); // Number of connected devices
uint8_t** get_connected_MACS(); // List of all connected devices, either clients or servers
uint8_t get_type_connection(uint8_t conn_id); // Returns CLIENT or SERVER depending on the link with conn_id

uint8_t* get_connid_MAC(uint8_t conn_id); // Returns the MAC of a connected device conn_id
uint8_t get_MAC_connid(uint8_t* mac_addr); // Returns the conn_id from a MAC of a connected device
uint8_t* get_my_MAC(); // Returns the MAC address of the device calling the function
uint8_t MAC_check(uint8_t* mac1, uint8_t* mac2); // 1 if mac1==mac2, 0 otherwise

uint8_t get_node_type(); // Returns CLIENT/SERVER
bool is_advertising(); // If the device is a server, returns true if it is advertising, false otherwise
bool is_scanning();	// If the device is a client, returns true if it is scanning. If the device is a server it may be scanning, looking for other servers to connect with.

uint8_t get_gatt_if(); //  Returns the interface of the device calling it, either server or client
uint8_t get_client_connid(); // Returns the conn_id that is assigned from the client to a server (if exists)

// Returns an array of 1 and 0. Position i has value 1 if i is an assigned conn_id, 0 otherwise.
// The array has length TOTAL_NUMBER_LIMIT. Use get_type_connection to know if a conn_id refers to a server or a client
uint8_t* get_server_connids(); 


// INTERNAL CLIENTS FUNCTIONS
// These functions have to be executed if the device is a server
// and the internal client of interest has been registered using register_internal_client(client_num);
// Client ids are: SERVER_S1, SERVER_S2, SERVER_S3
uint8_t get_internal_client_connid(uint8_t client_id);
uint8_t get_internal_client_gattif(uint8_t client_id);
uint8_t* get_internal_client_serverMAC(uint8_t client_id); // Returns the MAC address of the server which is connected to


// CALLBACKS
uint8_t install_NotifyCb(NotifyCb cb); // Returns 0 on succes, 1 otherwise
uint8_t install_InitCb(InitCb cb); //Same as above.
uint8_t install_ServerUpdateCb(ServerUpdateCb cb); //Same as above
uint8_t install_ExchangeRoutingTableCb(ExchangeRoutingTableCb cb); //Same as above
uint8_t install_ReceivedPacketCb(ReceivedPacketCb cb); //Same as above
uint8_t install_ShutDownCb(ShutDownCb cb); //Same as above
uint8_t install_SendRoutingTableCb(SendRoutingTableCb cb); //Same as above.

uint8_t install_EndScanning(EndScanning cb); // Triggered when the scan process of a client is over (including an internal_client)
uint8_t install_ServerLost(ServerLost cb); // Triggered when the client is connected to a server and the connection is lost

uint8_t install_SSC_Active(SSC_Active cb);
uint8_t install_SSC_Passive(SSC_Passive cb);

bool has_ended_scanning();

