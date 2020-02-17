//
// Created by thecave3 on 15/02/20.
//

#include "device.hpp"

class Device {
private:
    uint_8 timeout_sec = 5;
    Role role;
    bool connected = false;

    /**
     * Scan environment function
     */
    void scan_environment() {
        // scan timeout + service_guid
    }

    /**
     * Order function of the scan results  to attempt a connection with the neareast possible (higher RSSI).
     * The order algorithm used is an insertion sort since we expect that the list will be almost completely ordered.
     *
     * @param device_list list of the devices found in the scan
     * @return the first element of the list
     */
    bemesh_dev_t* select_device_to_connect(bemesh_dev_t *device_list, int length) {
        int i, j;
        bemesh_dev_t temp;

        for (i = 1; i < length; i++) {
            temp = device_list[i];
            j = i - 1;
            while (j >= 0 && device_list[j]->rssi > temp->rssi) {
                device_list[j + 1] = device_list[j];
                j--;
            }
            device_list[j + 1] = temp;
        }

        return device_list[0];
    }

    /**
     * Tries to connect to a server in order to become a client of that specific server
     *
     * @param target_server device representing the active server previously discovered
     * @return true if connected
     */
    bool connect_to_server(bemesh_dev_t target_server) {

        // TODO connection logic

        return false;
    }

public:

    guuid_t service_guuid;

    void setRole(Role role) {
        this->role = role;
    }

    Role getRole() {
        return this->role;
    }

    void setConnected(bool connected) {
        this->connected = connected;
    }

    bool isConnected() {
        return this->connected;
    }

    /**
     * Launcher function of the device
     */
    void startup() { scan_enviroment(); }

    /**
     * Operations performed as a server;
     */
    void server_routine() {}

    /**
     * Operations performed as a client;
     */
    void client_routine() {}


    /**
     * Callback to be launched after a scan has been completed
     *
     * @param device_list array representing the devices found during the can
     * @param list_length length of the array
     */
    void on_scan_completed(bemesh_dev_t *device_list, int list_length) {

        bemesh_dev_t* target = select_device_to_connect(device_list, list_length);
        this->role = Role::CLIENT;

        for (int i = 0; !connected && i < list_length; i++, target = device_list[i + 1])
            connected = connect_to_server(target);

        if (connected)
            client_routine();
        else {
            this->role = Role::SERVER;
            server_routine();
        }

    }

    /**
     * Callback triggered when a message is received to this device. Parameters to be decided.
     *
     */
    void on_message_received(/*id or something to identify the sender*/,/*message*/){

    }

    /**
     *  Send a message to a particular device. Parameters to be decided.
     *
     */
    void send_message(/*id or something to identify the receiver*/,/*message*/){

    }

    /**
     * Callback triggered when a message of update  of the routing table is received. Parameters to be decided.
     */
    // TODO valutare se è utile altriment eliminare e includere tutto nella on message received.
    void on_routing_table_update(/*update defined by you in message protocol*/);
};