#include "consumer_order.h"
#include "items.h"


void * consumer_order::consumer(void * arg){
    Item * items = (Item *) arg;
    int num_consumed = 0;
    while (num_consumed < (*items).n){
        std::this_thread::sleep_for(std::chrono::seconds((*items).avg_time));
        std::cout << "Consuming order: " << ++num_consumed << " Order Type: " << (*items).type << std::endl;
        //signal to main to become unblocked and terminate
    }
};