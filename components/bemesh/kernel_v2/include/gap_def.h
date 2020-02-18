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
