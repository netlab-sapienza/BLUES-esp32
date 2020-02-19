/*
 * gap_def.h
 */

#pragma once

// Duration of the SCAN procedure in seconds
#define SCAN_DURATION_SEC 10

// Operative modes for gap_handler.
// Peripheral -> advertising mode
// Central -> scanning mode
#define GAP_HANDLER_MODE_PERIPHERAL 0
#define GAP_HANDLER_MODE_CENTRAL 1

// Maximum savable devices during scan
#define GAP_HANDLER_SCAN_DEVS_MAX 32

// Iniquiry response code for Complete 128bit service uuid
#define GAP_SRV_ER 0x07
// Length and definition of advertised uuid.
#define GAP_SRV_UUID_LEN 16
extern uint8_t bemesh_gap_service_uuid[GAP_SRV_UUID_LEN];
