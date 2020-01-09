
#pragma once
#define MASTER_TX_BUFFER_SIZE 512
#define SLAVE_TX_BUFFER_SIZE 512
#define MAC_ADDRESS_SIZE 6
#define UPDATE_ADD_CLIENT 0
#define UPDATE_REMOVE_CLIENT 1
#define UPDATE_ADD_SERVER 2
#define UPDATE_REMOVE_SERVER 3
#define WRITE_TASK_STACK_SIZE 20480 //20 bytes stack allocated for the task.
#define TASK_PRIORITY 5 //See https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/freertos.html
#define MASTER_EXTRA_ARGS_BUFFER_SIZE 256
#define SLAVE_EXTRA_ARGS_BUFFER_SIZE 256
