/*
 * core_int.c
 */

#include "core_int.h"
#include "core.h"

static bemesh_core_t *core;

/*
 * Install the cb callback for the Event event.
 * Parameters will be updated before launching the cb
 * Refer to bemesh_kernel_evt_t enum to check how events are defined.
 * Refer to bemesh_evt_params_t union to check how params are passed.
 */
void kernel_install_cb(bemesh_kernel_evt_t event, kernel_cb cb);

/*
 * Initializes the underlying kernel.
 * This function has to be called in the startup of the system.
 */
int kernel_init(void) {
  // initializes the core structure.
  core = bemesh_core_init();
  if (!core) {
    // If something goes wrong, return an error
    return 1;
  }
  return 0;
}


static bda_id_tuple *find_tuple(bda_id_tuple* vect,
                                uint8_t len, esp_bd_addr_t bda,
                                uint8_t *found_flag) {
  for (int i = 0; i < len; ++i) {
    bda_id_tuple *entry = vect[i];
    if (bda_equals(bda, entry->bda)) {
      *found_flag = true;
      return entry;
    }
  }
  *found_flag = false;
  return NULL;
}

/**
 *  Transfer the src buffer of len bytes to another device with bda address.
 *
 * @param bda address of the device that has to receive the payload
 * @param src array that contains the payload written in byte
 * @param len length of the payload
 */
void send_payload(esp_bd_addr_t bda, uint8_t *src, uint16_t len) {
  
}


/**
 * Tries to connect to a specific device
 *
 * @param bda address
 * @return 0 if no error occurred
 */
uint8_t connect_to(esp_bd_addr_t bda);

/**
 * Scan the environment
 *
 * @param timeout length of the scan in seconds
 */
void scan_environment(uint8_t timeout);

/**
 * Get the device bda. It can be passed to bemesh::to_dev_addr() in order to get the correct address.
 *
 * @return the device bda in bytes
 */
uint8_t *get_own_bda(void);
