#include "../include/bleClient.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "semaphore.h"



sem_t sem;
int ret = sem_init(&sem,0,1);


void thread_example_function(int thread_id){



    //Raw approach for thread safety. To be improved.
    sem_wait(&sem);
    std::cout<<"Hello: I'm thread: "<<thread_id<<std::endl;
    BleClient* ble_client = BleClient::getIstance();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    sem_post(&sem);
    return;

}




int main(int argc, char** argv){

    std::cout<<"Test started"<<std::endl;
    
    std::thread threads[10];
    for(int i = 0; i<10; i++){
        threads[i] = std::thread(&thread_example_function, i);
    }

    for(int i = 0; i<10; i++){
        threads[i].join();
    }
    std::cout<<"Threads terminated"<<std::endl;

    /*

    std::cout<<"Now we test bleClient initialization"<<std::endl;
    BleClient* ble_client = BleClient::getIstance();
    ble_client = ble_client->getIstance();
    ble_client = ble_client->getIstance();
    

    ble_client->close();
    */




    return 0;
}
