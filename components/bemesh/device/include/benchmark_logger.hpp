/**
 * benchmark_logger.hpp
 *
 * simple logger wrapper for logging high level events
 * under the data-collector standard.
 */

#pragma once

#include "bemesh_messages_v2.hpp"
#include "bemesh_status.hpp"
#include "routing.hpp"
#include "device.hpp"

using namespace bemesh;

namespace benchmark {
  /**
   * Logger for outgoing messages.
   *
   * @param h pointer to the outgoing message
   * @hop_bda bda of the device whose receiving the message.
   */
  void log_outgoing_message(MessageHeader *h,
			    const dev_addr_t &hop_bda);
  
  /**
   * Logger for incoming messages.
   *
   * @param h pointer to the incoming message
   * @hop_bda bda of the device who sent the message
   */
  void log_incoming_message(MessageHeader *h,
			    const dev_addr_t &hop_bda);

  /**
   * Logger for outgoing connections.
   *
   * @param bda bda of the device whose receiving connection
   */
  void log_outgoing_connection(const dev_addr_t &bda);

  /**
   * Logger for incoming connections.
   *
   * @param bda bda of the device whose receiving connection
   */
  void log_incoming_connection(const dev_addr_t &bda);

  /**
   * Logger for connection status
   * 
   * @param bda bda of the device related in the connection
   * @param status 1 for accepted, 0 for refused.
   * @param conn_dir 0 for incoming, 1 for outgoing
   */
  void log_status_connection(const dev_addr_t &bda,
			     uint8_t status,
			     uint8_t conn_dir);

  /**
   * Logger for device initialization.
   */
  void log_device_up(void);

  /**
   * Logger for server/client role change.
   *
   * @param r role currently set for the device.
   */
  void log_role(Role r);

  /**
   * Logger for scan status
   *
   * @param status 1 for start-scan, 0 for stop-scan
   */
  void log_scan(uint8_t status);

  /**
   * Logger for routing table.
   * to be launched every time a message that can alter the
   * routing table is transmitted/received
   *
   * @param h pointer to the involved message
   * @param hop_bda bda on which to transmit/receive the message
   * @param inst instance of the device class
   * @param sent flag to indicate if the message is sent or received.
   *        1 for sent, 0 for received
   */
  void log_routing_table(MessageHeader *h,
			 const dev_addr_t &hop_bda,
			 const Device& inst,
			 uint8_t sent);
}
 
