/*
 * gatt_def.h
 */

#pragma once
#include <stdint.h>
#include "esp_bt_defs.h"

/*
 * Generic definitions
 */
#define GATT_CHAR_BUF_SIZE 256
#define GATT_MAX_CONNECTIONS 7 // Maximum no. of connections (outgoing+incoming) of the current dev.

/*
 * GATT-Server (GATTS)
 */
#define GATTS_APP_ID 0
#define GATTS_NUM_HANDLES 1

#define GATTS_SERV_UUID 0xDEAD // Generic Access UUID for main service of Bemesh
#define GATTS_CHAR_UUID 0xBEB0 // Digital UUID for characteristic (n 2-bit values stored in the characteristic.)
#define GATTS_MAX_CONNECTIONS 0x04 // Maximum incoming connections possible.

/*
 * GATT-Client (GATTC)
 */
#define GATTC_APP_ID 1
#define GATTC_MTU_SIZE 256
#define GATTC_APP_PROFILE_INST_LEN 4
#define GATTC_MAX_CONNECTIONS 0x03 // Maximum outgoing connections possible.

#if GATTS_MAX_CONNECTIONS + GATTC_MAX_CONNECTIONS > GATT_MAX_CONNECTIONS
#error "ERROR: outgoing connections + incoming connections exceed maximum possible connections"
#endif

