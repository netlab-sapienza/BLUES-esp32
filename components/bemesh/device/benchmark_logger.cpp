/**
 * benchmark_logger.cpp
 *
 * simple logger wrapper for logging high level events
 * under the data-collector standard.
 */

#include "benchmark_logger.hpp"
#include "device.hpp"
#include <cstring> // memset
extern "C" {
#include "core_int.h" // get_own_bda()
}

/**
 * print on the region pointed by buf, the device bda.
 * The format for the bda print is:
 * - "AABBCCDDEEFF" 
 * No dots are used to separate the bytes.
 *
 * @param buf pointer to an array of chars.
 * @param bda reference to the bda to print
 *
 * @return no. of written bytes on buf.
 */
static int _print_bda(char *buf, const dev_addr_t &bda) {
  int wb=0;
  for (int i = 0; i < ESP_BD_ADDR_LEN; ++i) {
    wb+=sprintf(buf+wb, "%02X.", bda[i]);
  }
  return wb;
}

static void _reset_buffer(char *buf, uint16_t len) {
  memset(buf, 0, len);
  return;
}

namespace benchmark {
  
#define BENCHMARK_BUF_LEN 512
  // buffer on which to generate the log message
  static char log_buf[BENCHMARK_BUF_LEN];
  static const char *TAG = "BenchMark";

  // Event ID table, according to the
  // data-collector definition.
  enum event_type_t {
    MessageSent=0,
    MessageRecv=1,
    OutgoingConn=2,
    IncomingConn=3,
    ConnStatus=4,
    DeviceStatus=5,
    DeviceRole=6,
    DeviceScan=7,
  };
  
  /**
   * Logger for outgoing messages.
   *
   * @param h pointer to the outgoing message
   * @hop_bda bda of the device whose receiving the message.
   */
  void log_outgoing_message(MessageHeader *h,
			    const dev_addr_t &hop_bda) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    wb += sprintf(buf+wb, " %d ",
		  event_type_t::MessageSent);
    wb += _print_bda(buf+wb, h->source());
    wb += sprintf(buf+wb, " ");
    wb += _print_bda(buf+wb, h->destination());
    wb += sprintf(buf+wb, " ");
    wb += _print_bda(buf+wb, hop_bda);
    wb += sprintf(buf+wb, " %d NULL ",
		  h->id());
    uint8_t *payload = (uint8_t *)h->payload_ptr();
    if(payload != nullptr) {
      for(int i = 0; i < h->psize(); ++i) {
	wb += sprintf(buf+wb, "%02X", payload[i]);
      }
    }
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }
  
  /**
   * Logger for incoming messages.
   *
   * @param h pointer to the incoming message
   * @hop_bda bda of the device who sent the message
   */
  void log_incoming_message(MessageHeader *h,
			    const dev_addr_t &hop_bda) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    wb += sprintf(buf+wb, " %d ",
		  event_type_t::MessageRecv);
    wb += _print_bda(buf+wb, h->source());
    wb += sprintf(buf+wb, " ");
    wb += _print_bda(buf+wb, h->destination());
    wb += sprintf(buf+wb, " ");
    wb += _print_bda(buf+wb, hop_bda);
    wb += sprintf(buf+wb, " %d NULL ",
		  h->id());
    uint8_t *payload = (uint8_t *)h->payload_ptr();
    if(payload != nullptr) {
      for(int i = 0; i < h->psize(); ++i) {
	wb += sprintf(buf+wb, "%02X", payload[i]);
      }
    }
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for outgoing connections.
   *
   * @param bda bda of the device whose receiving connection
   */
  void log_outgoing_connection(const dev_addr_t &bda) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    wb += sprintf(buf+wb, " %d ",
		  event_type_t::OutgoingConn);
    wb += _print_bda(buf+wb, bda);
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for incoming connections.
   *
   * @param bda bda of the device whose receiving connection
   */
  void log_incoming_connection(const dev_addr_t &bda) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    wb += sprintf(buf+wb, " %d ",
		  event_type_t::IncomingConn);
    wb += _print_bda(buf+wb, bda);
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for connection status
   * 
   * @param bda bda of the device related in the connection
   * @param status 1 for accepted, 0 for refused.
   * @param conn_dir 0 for incoming, 1 for outgoing
   */
  void log_status_connection(const dev_addr_t &bda,
			     uint8_t status,
			     uint8_t conn_dir) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    wb += sprintf(buf+wb, " %d ",
		  event_type_t::ConnStatus);
    
    if(conn_dir) {
      // outgoing connection: requester is this device
      wb += _print_bda(buf+wb, own_bda);
      wb += sprintf(buf+wb, " ");
      wb += _print_bda(buf+wb, bda);
    } else {
      // incoming connection: requester is bda
      wb += _print_bda(buf+wb, bda);
      wb += sprintf(buf+wb, " ");
      wb += _print_bda(buf+wb, own_bda);
    }

    char status_c = (status) ? 'A' : 'R';
    wb += sprintf(buf+wb, " %c", status_c);
    ESP_LOGI(TAG, "%s", buf);
    
    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for device initialization.
   */
  void log_device_up(void) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    sprintf(buf+wb, " %d", event_type_t::DeviceStatus);
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for server/client role change.
   *
   * @param r role currently set for the device.
   *        1 for server, 2 for client
   */
  void log_role(Role r) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    char r_c = ((int)r == 1) ? 'S' : 'C'; 
    
    sprintf(buf+wb, " %d %c",
	    event_type_t::DeviceRole,
	    r_c);
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for scan status
   *
   * @param status 1 for start-scan, 0 for stop-scan
   */
  void log_scan(uint8_t status) {
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    char status_c = (status == 1) ? 'S' : 'E';

    sprintf(buf+wb, " %d %c",
	    event_type_t::DeviceScan,
	    status_c);
    ESP_LOGI(TAG, "%s", buf);

    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }

  /**
   * Logger for routing table.
   * to be launched every time a message that can alter the
   * routing table is transmitted/received
   *
   * @param h pointer to the involved message
   * @param hop_bda bda on which to transmit/receive the message
   * @param inst instance of the device class
   * @param sent flag to indicate if the message is sent or received.
   */
  void log_routing_table(MessageHeader *h,
			 const dev_addr_t &hop_bda,
			 const Device& inst,
			 uint8_t sent) {
    // Follows the same format for the received/sent message
    char *buf = log_buf;
    dev_addr_t own_bda =
      to_dev_addr(get_own_bda());
    int wb = 0;
    wb = _print_bda(buf, own_bda);
    if(sent) {
      wb += sprintf(buf+wb, " %d ",
		    event_type_t::MessageSent);
    } else {
      wb += sprintf(buf+wb, " %d ",
		    event_type_t::MessageRecv);
    }
    wb += _print_bda(buf+wb, h->source());
    wb += sprintf(buf+wb, " ");
    wb += _print_bda(buf+wb, h->destination());
    wb += sprintf(buf+wb, " ");
    wb += _print_bda(buf+wb, hop_bda);
    wb += sprintf(buf+wb, " %d NULL ",
		  h->id());

    auto rtable = inst.getRouter().getRoutingTable();
    for (auto &i : rtable) {
      wb += _print_bda(buf+wb, i.target_addr);
      wb += sprintf(buf+wb, "-");
    }
    wb += _print_bda(buf+wb, own_bda);
    ESP_LOGI(TAG, "%s", buf);
    
    _reset_buffer(buf, BENCHMARK_BUF_LEN);
    return;
  }
}
