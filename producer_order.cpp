#include "producer_order.h"
#include "main.cpp"
#include "items.h"


void * producer_order::producer(void * arg){
    Item * items = (Item *) arg;
    int num_produced = 0;
    while (num_produced < (*items).n){
        std::this_thread::sleep_for(std::chrono::seconds((*items).avg_time));
        std::cout << "Producing order: " << ++num_produced << " Order Type: " << (*items).type << std::endl;
    }
};

