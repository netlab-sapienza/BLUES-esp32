
#pragma once
#define MASTER_TX_BUFFER_SIZE 512
#define SLAVE_TX_BUFFER_SIZE 512
#define MAC_ADDRESS_SIZE 6
#define UPDATE_ADD_CLIENT 0
#define UPDATE_REMOVE_CLIENT 1
#define UPDATE_ADD_SERVER 2
#define UPDATE_REMOVE_SERVER 3
#define WRITE_TASK_STACK_SIZE 81920 //80 bytes stack allocated for the task.
#define TASK_PRIORITY 5 //See https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/freertos.html
#define MASTER_EXTRA_ARGS_BUFFER_SIZE 256
#define SLAVE_EXTRA_ARGS_BUFFER_SIZE 256
#define PING_RESPONSE_NTF_SIZE  50

#define PONG_FLAG_VALUE 1
#define PING_FLAG_VALUE 0

#define NOTIFY_YES 1
#define NOTIFY_NO 0


//Sleep time is 3 seconds
#define SLEEP_TIME 3


#define INTERNAL_CLIENT_FLAG 1
#define CLIENT_FLAG 0


#define CLIENT_SERVER 0
#define SERVER_SERVER 1

