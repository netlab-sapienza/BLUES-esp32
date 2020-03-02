//
// Created by thecave3 on 24/02/20.
//

#include "device_callbacks.hpp"
#include "device.hpp"
#include <cstring>

extern "C" {
#include <esp_log.h>
#include <gatt_def.h>
}

#define TIMEOUT_DELAY 10
static const char *TAG = "device_callbacks";

using namespace bemesh;

bemesh_dev_t *filter_devs_rtable(Device &instance, bemesh_dev_t *src,
                                 uint16_t src_len, uint16_t *t_dest_len) {
  ESP_LOGV(TAG, "starting filter proc.");
  auto *dest = (bemesh_dev_t *)malloc(sizeof(bemesh_dev_t) * src_len);
  uint16_t dest_len = 0;
  for (int i = 0; i < src_len; ++i) {
    ESP_LOGV(TAG, "Filtering elem %d.", i);
    bool ret = instance.getRouter().contains(bemesh::to_dev_addr(src[i].bda));
    if (!ret) {
      ESP_LOGV(TAG, "Keeping the following element.");
      memcpy(&dest[dest_len], &src[i], sizeof(bemesh_dev_t));
      dest_len++;
    }
  }
  *t_dest_len = dest_len;
  return dest;
}

/**
 * If the scan shown no results, this function is called.
 * the server should advertise for a certain period of time.
 * After that, the scanning procedure should be restarted.
 */
static void fsm_post_scan_server_routine(Device &inst) {
  ESP_LOGI(TAG, "fsm_post_scan_server_routine");
  // Start the advertising proc.
  if(get_num_inc_conn() < GATTS_MAX_CONNECTIONS) {
    ESP_LOGI(TAG, "Launching advertising.");
    // Advertise for DEVICE_TIMEOUT_ADV_MS
    start_advertising();
    vTaskDelay(inst.getAdvTimeout());
    stop_advertising();
  }
  // if no connections happened during this period,
  // launch the scan operation
  inst.scan_the_environment();
  return;
}

static void fsm_scan_cmpl_no_devs_routine(Device &inst) {
  ESP_LOGI(TAG, "no device found during scan.");
  // Execute advertising procedure.
  inst.setState(DeviceState::Advertising);
  // Set the device as server
  inst.setRole(Role::SERVER);
  // Launch the post scan procedure.
  fsm_post_scan_server_routine(inst);
  return;
}

static void fsm_routing_dis_sendreq(Device &inst,
				    dev_addr_t remote_bda) {
  // TODO(Emanuele): Prepare the Routing discovery Request.
  dev_addr_t local_bda = to_dev_addr(get_own_bda());
  RoutingDiscoveryRequest req_msg =
    RoutingDiscoveryRequest(remote_bda, local_bda);
  ESP_LOGI(TAG, "Sending RoutingDiscoveryRequest message.");
  inst.send_message(&req_msg);
  // Set the state of the device.
  inst.setState(DeviceState::RTClientSentReq);
  return;
}

static void fsm_scan_cmpl_dev_found(Device &inst,
				    bemesh_dev_t *conn_target) {
  ESP_LOGI(TAG, "fsm_scan_cmpl_dev_found");
  // Connection established with the conn_target.
  // Add it to the routing table and start discovery request
  bemesh::dev_addr_t conn_bda = bemesh::to_dev_addr(conn_target->bda);
  ESP_LOGI(TAG, "Adding new entry in routing table.");
  inst.getRouter().add(conn_bda, conn_bda,
		       0, bemesh::RoutingFlags::Reachable);
  // Start the routing discovery procedure
  fsm_routing_dis_sendreq(inst, conn_bda);
}

void fsm_scan_cmpl(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "fsm_scan_cmpl");
  Device &inst = Device::getInstance();
  uint16_t res_len=params->scan.len;
  // If no results are present, stop the callback now.
  if(!res_len) {
    fsm_scan_cmpl_no_devs_routine(inst);
    return;
  }
  // at least one device is present.
  // filter the results
  bemesh_dev_t *src_devs = params->scan.result;
  uint16_t flt_devs_len;
  bemesh_dev_t *flt_devs = filter_devs_rtable(inst, src_devs,
					      res_len,
					      &flt_devs_len);
  // If no results are present in the filtered list, stop the callback
  if(!flt_devs_len) {
    free(flt_devs);
    fsm_scan_cmpl_no_devs_routine(inst);
    return;
  }
  ESP_LOGI(TAG, "scan result contains %d entries.",
	   flt_devs_len);

  // Sort the flt_devs
  bemesh_dev_t *sorted_flt_devs =
    Device::select_device_to_connect(flt_devs, flt_devs_len);
  // reset the connection flag.
  inst.setConnected(false);
  // Attempt connection on the scan result list.
  for (int i = 0; i < flt_devs_len; ++i) {
    ESP_LOGI(TAG, "Establishing connection with entry no.%d.", i);
    bemesh_dev_t *conn_target = &sorted_flt_devs[i];
    // lock the connection semaphore until the connection is established
    inst.setState(DeviceState::Connecting);
    inst.connect_to_server(conn_target->bda);
    xSemaphoreTake(inst.getConnectionSemaphore(), portMAX_DELAY);
    // to reach this place, connection must either been accepted or refused.
    if(inst.isConnected()) {
      // Connection was established.
      ESP_LOGI(TAG, "Connection succesful.");
      fsm_scan_cmpl_dev_found(inst, conn_target);
      free(flt_devs);
      return;
    } else {
      ESP_LOGI(TAG, "Connection failed. trying the next entry.");
    }    
  }
  
  free(flt_devs);
  fsm_scan_cmpl_no_devs_routine(inst);
  return;
}

void fsm_outgoing_conn_cmpl(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "fsm_outgoing_conn_cmpl");
  Device &inst = Device::getInstance();
  if(params->conn.ack) {
    ESP_LOGI(TAG, "Connection succesful.");
    inst.setConnected(true);
  } else {
    inst.setConnected(false);
  }
  // Free the connection semaphore
  xSemaphoreGive(inst.getConnectionSemaphore());
  return;
}

/**
 * Incoming connection handler.
 * Add the connection to the routing table.
 */
void fsm_incoming_conn_cmpl(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "fsm_incoming_conn_cmpl");
  Device &inst = Device::getInstance();
  // get the remote bda address.
  dev_addr_t remote_bda = to_dev_addr((uint8_t *)params->conn.remote_bda);
  // add the new connection to the router.
  inst.getRouter().add(remote_bda, remote_bda, 0, Reachable);

  // If we still have space in the incoming connection pool, advertise.
  /*
    if(get_num_inc_conn() < GATTS_MAX_CONNECTIONS) {
      start_advertising();
    }
  */
  // The section has been commented as its better to maintain advertising happening
  // only in the 
  return;
}

static void fsm_redirect_msg(Device &inst,
			     MessageHeader *msg) {
  //TODO(Emanuele): Complete the redirect (or hop) function.
  return;
}

/**
 * Incoming message routines.
 * Each function will handle a different incoming message.
 * Refer to the end of the file to see the implementation.
 */
static void fsm_msg_recv_routing_disreq(Device &inst,
				 RoutingDiscoveryRequest *req_msg);
static void fsm_msg_recv_routing_disres(Device &inst,
				 RoutingDiscoveryResponse *res_msg);
static void fsm_msg_recv_routing_update(Device &inst,
				 RoutingUpdateMessage *up_msg);
// TODO(Emanuele): Complete the remaining messages.
/**
 * Message receive handler.
 * Must handle messages coming from the lower levels.
 */
void fsm_msg_recv(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "fsm_msg_recv");
  Device &inst = Device::getInstance();
  // get the payload from the params.
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;

  // Parse the message
  // TODO(Emanuele): Free the message when its purpose terminates.
  MessageHeader* _msg =
    MessageHandler::getInstance().unserialize(payload, payload_len);
  // If the current device is not the destinatary of the message, forward
  // it through the routing table.
  if(_msg->destination() != to_dev_addr(get_own_bda())) {
    fsm_redirect_msg(inst, _msg);
    return;
  }

  switch(_msg->id()) {
  case ROUTING_DISCOVERY_REQ_ID: {
    fsm_msg_recv_routing_disreq(inst, (RoutingDiscoveryRequest *)_msg);
    break;
  }
  case ROUTING_DISCOVERY_RES_ID: {
    fsm_msg_recv_routing_disres(inst, (RoutingDiscoveryResponse *)_msg);
    break;
  }
  }
}

void on_scan_completed(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Starting OnScanComplete operation: len: %d", params->scan.len);
  bemesh_dev_t *device_list = params->scan.result;
  uint16_t list_length = params->scan.len;
  Device &instance = Device::getInstance();

  // connection target which will be copied from the params
  // used to add an entry in the routing table.
  bemesh_dev_t conn_target;

  kernel_uninstall_cb(ON_SCAN_END);

  // Filter the scan result based on the current routing table. All the
  // devices that are advertising, but are currently already present in the
  // table, are removed from the scan result.
  uint16_t f_device_list_len = 0;
  bemesh_dev_t *f_device_list = filter_devs_rtable(
      instance, device_list, list_length, &f_device_list_len);
  device_list = f_device_list;
  list_length = f_device_list_len;
  ESP_LOGI(TAG, "%d entries after filtering.", list_length);

  if (list_length > 0) {
    bemesh_dev_t *target =
        Device::select_device_to_connect(device_list, list_length);
    // copy the target in the conn_target support object.
    memcpy(&conn_target, target, sizeof(bemesh_dev_t));
    kernel_install_cb(ON_OUT_CONN, on_connection_response);
    for (int i = 0; !instance.isConnected() && i < list_length;
         i++, *target = device_list[i + 1]) {
      ESP_LOGI(TAG, "Attempt to connect to server.");
      instance.connect_to_server(target->bda);
      ESP_LOGI(TAG, "Locking the connection semaphore.");
      xSemaphoreTake(instance.getConnectionSemaphore(), portMAX_DELAY);
      ESP_LOGI(TAG, "Semaphore unlocked.");
    }
  }
  // Free the filtered device list
  free(f_device_list);

  if (instance.isConnected()) {
    ESP_LOGI(TAG, "onscancmpl: starting client routine.");
    // Add the new entry to the routing table
    auto device = to_dev_addr(conn_target.bda);
    instance.getRouter().add(device, device, 0, Reachable);
    if (instance.getRole() == Role::UNDEFINED) {
      // Launch client routine
      instance.client_routine();
    }
    if (instance.getRole() == Role::SERVER) {
      // Link Server <-> Server
      RoutingDiscoveryRequest request =
          RoutingDiscoveryRequest(device, to_dev_addr(get_own_bda()));
      ESP_LOGI(TAG, "Sending routing discovery request to:");
      ESP_LOG_BUFFER_HEX(TAG, device.data(), 6);

      ErrStatus ret = instance.send_message(&request);
      if (ret != Success) {
        ESP_LOGE(TAG, "Something went wrong on the send message!");
      }
    }
  } else {
    ESP_LOGI(TAG, "onscancmpl: starting server routine.");
    instance.addTimeoutSec(TIMEOUT_DELAY);
    if (instance.getRole() == Role::UNDEFINED) {
      instance.server_first_routine();
    } else {
      // i am already a server i do not need to instantiate the callbacks
      // again
      instance.server_routine();
    }
  }
}

void on_connection_response(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Connection response callback.");
  Device &instance = Device::getInstance();

  if (instance.getRole() == Role::UNDEFINED) {
    ESP_LOGI(TAG, "Starting undefined role routine.");
    if (params->conn.ack) {
      ESP_LOGI(TAG, "Setting connected flag to true.");
      instance.setConnected(true);
    }
  } else if (instance.getRole() == Role::SERVER) {
    ESP_LOGI(TAG, "Starting server role routine.");
    RoutingDiscoveryRequest request = RoutingDiscoveryRequest(
        to_dev_addr(params->conn.remote_bda), to_dev_addr(get_own_bda()));
    ESP_LOGI(TAG, "Sending routing discovery request.");
    instance.send_message(&request);
  }
  xSemaphoreGive(instance.getConnectionSemaphore());
}

void on_incoming_connection(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Incoming connection callback.");
  Device &instance = Device::getInstance();
  auto remote_bda = params->conn.remote_bda;
  auto device = to_dev_addr((uint8_t *)remote_bda);
  uint8_t t_num_hops = 0;
  uint8_t t_flag = Reachable;
  instance.getRouter().add(device, device, t_num_hops, t_flag);

  if (get_num_inc_conn() < GATTS_MAX_CONNECTIONS)
    start_advertising();
}

void on_message_received(bemesh_evt_params_t *params) {
  ESP_LOGI(TAG, "Message received callback.");
  //  auto sender = to_dev_addr((uint8_t *)params->recv.remote_bda);
  uint8_t *payload = params->recv.payload;
  uint16_t payload_len = params->recv.len;
  Device &instance = Device::getInstance();
  MessageHandler handler = MessageHandler::getInstance();
  MessageHeader *message = handler.unserialize(payload, payload_len);
  // DEBUG ONLY (Print the message header)
  // ESP_LOGI(TAG, "@@@ Printing received message header @@@");
  // ESP_LOGI(TAG, "Destination:");
  // ESP_LOG_BUFFER_HEX(TAG, message->destination().data(), ESP_BD_ADDR_LEN);
  // ESP_LOGI(TAG, "Source:");
  // ESP_LOG_BUFFER_HEX(TAG, message->source().data(), ESP_BD_ADDR_LEN);
  // ESP_LOGI(TAG, "Payload size: %d", message->psize());

  if (message->destination() != to_dev_addr(get_own_bda())) {
    ESP_LOGI(TAG, "This message is not for me.");
  }
  switch (message->id()) {
  case ROUTING_DISCOVERY_REQ_ID: {
    if (instance.getRole() == Role::SERVER) {
      ESP_LOGI(TAG, "Received RoutingDiscoveryRequest");
      std::vector<routing_params_t> routing_table =
          instance.getRouter().getRoutingTable();
      RoutingDiscoveryResponse response = RoutingDiscoveryResponse(
          message->source(), to_dev_addr(get_own_bda()), routing_table,
          routing_table.size());
      // ESP_LOGI(TAG, "@@@ Printing outgoing message header @@@");
      // ESP_LOGI(TAG, "Destination:");
      // ESP_LOG_BUFFER_HEX(TAG, response.destination().data(),
      // ESP_BD_ADDR_LEN); ESP_LOGI(TAG, "Source:"); ESP_LOG_BUFFER_HEX(TAG,
      // response.source().data(), ESP_BD_ADDR_LEN); ESP_LOGI(TAG, "Payload
      // size: %d", response.psize()); ESP_LOGI(TAG, "Preparing to send
      // response.");
      instance.send_message(&response);
    }
    break;
  }
  case ROUTING_DISCOVERY_RES_ID: {
    ESP_LOGI(TAG, "Received routing discovery response");
    ESP_LOG_BUFFER_HEX(TAG, message->source().data(), ESP_BD_ADDR_LEN);
    auto *res_packet = (RoutingDiscoveryResponse *)message;
    for (int i = 0; i < res_packet->entries(); ++i) {
      auto &entry = res_packet->payload()[i];
      instance.getRouter().add(entry);
    }
    break;
  }
  case ROUTING_PING_ID: {
    ESP_LOGI(TAG, "ping received");
    break;
  }
  case ROUTING_SYNC_ID: {
    ESP_LOGI(TAG, "routing sync id");
    break;
  }
  case ROUTING_UPDATE_ID: {
    ESP_LOGI(TAG, "update id");
    break;
  }
  default: {
    ESP_LOGE(TAG, "Cannot identify message");
  }
  }

  // if i am the target of the message i'll log it.
  // otherwise i forward the message to the address that the routing table gives
  // me
}

// Utility function to send a RoutingDiscoveryResponse message.
static void fsm_send_rd_res(Device &inst,
			    dev_addr_t dest) {
  // Serialize the routing table.
  std::vector<routing_params_t> rtable_vect =
    inst.getRouter().getRoutingTable();
  
  // Generate the routing discovery response message
  RoutingDiscoveryResponse res_msg =
    RoutingDiscoveryResponse(dest,
			     to_dev_addr(get_own_bda()),
			     rtable_vect,
			     rtable_vect.size());
  ESP_LOGI(TAG, "Sending the following entries:");
  for(int i = 0; i < res_msg.entries(); ++i) {
    routing_params_t *entry = &res_msg.payload()[i];
    ESP_LOG_BUFFER_HEX(TAG, entry->target_addr.data(), ESP_BD_ADDR_LEN);
  }
  ESP_LOGI(TAG, "End.");
  // Send the response
  ESP_LOGI(TAG, "Sending RoutingDiscoveryResponse");
  inst.send_message(&res_msg);
  return;
}

/**
 * Incoming message routines.
 * Each function will handle a different incoming message.
 * Refer to the end of the file to see the implementation.
 */
// Routing Discovery Request.
static void fsm_msg_recv_routing_disreq(Device &inst,
					RoutingDiscoveryRequest *req_msg) {
  ESP_LOGI(TAG, "Received RoutingDiscoveryRequest.");
  // Send the RoutingDiscoveryResponse
  fsm_send_rd_res(inst, req_msg->source());
  // Set the state of the device.
  inst.setState(DeviceState::RTServerRecvReq);
  return;
}

static void fsm_post_routing_discovery_routine(Device &inst);

static void fsm_msg_recv_routing_disres(Device &inst,
					RoutingDiscoveryResponse *res_msg) {
  ESP_LOGI(TAG, "Received RoutingDiscoveryResponse");
  // The current device will accept the message iff it was in one of the
  // following states:
  // - RTClientSentReq : The client has sent a request, and its waiting for a response
  // - RTServerRecvReq : The server has received a request, and its waiting for a response
  DeviceState inst_state = inst.getState();
  if(inst_state != DeviceState::RTClientSentReq &&
     inst_state != DeviceState::RTServerRecvReq) {
    ESP_LOGW(TAG, "Warning, could not accept the routing discovery response.");
    return;
  }

  // If the device received a response after sending a request, it must send its
  // routing table.
  if(inst_state == DeviceState::RTClientSentReq) {
    ESP_LOGI(TAG, "Preparing discovery response for the server.");
    fsm_send_rd_res(inst, res_msg->source());
  }

  // Preprocess the new routing table.
  Router::preprocessRoutingTable(res_msg->source(),
				 res_msg->payload().data(),
				 res_msg->entries());

  // Include the payload of the message into the current device's routing table.
  for(int i = 0; i < res_msg->entries(); ++i) {
    auto &entry = res_msg->payload()[i];
    inst.getRouter().add(entry);
  }

  // Set the state of the device.
  inst.setState(DeviceState::RTFinished);
  // Launch the post routing discovery routine.
  fsm_post_routing_discovery_routine(inst);
  return;
}

static void fsm_msg_recv_routing_update(Device &inst,
					RoutingUpdateMessage *up_msg) {
  // TODO(Emanuele): Complete the function.
  return;
}

// After the routing discovery procedure terminates, the function is called.
static void fsm_post_routing_discovery_routine(Device &inst) {
  if(inst.getRole() == Role::SERVER) {
    // Launch the post-scanning routine to enter in the advertise-scan loop.
    fsm_post_scan_server_routine(inst);
  } else {
    inst.setRole(Role::CLIENT);
    // TODO(Emanuele, Andrea): Add some behaviour for the client at this point
  }
  return;
}
