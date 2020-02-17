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

/*
 * GATT-Server (GATTS)
 */
#define GATTS_APP_ID 0
#define GATTS_NUM_HANDLES 4

#define GATTS_SERV_UUID 0xDEAD // Generic Access UUID for main service of Bemesh
#define GATTS_CHAR_UUID 0xBEB0 // Digital UUID for characteristic (n 2-bit values stored in the characteristic.)

/*
 * GATT-Client (GATTC)
 */
#define GATTC_APP_ID 1
#define GATTC_MTU_SIZE 256
#define GATTC_APP_PROFILE_INST_LEN 4
