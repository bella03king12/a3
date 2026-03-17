#include "producer_order.h"
#include "items.h"
#include "order_queue.h"


void * producer(void * arg){
    Item * items = (Item *) arg;
    Order order;
    int num_produced = 0;
    while (num_produced < (*items).n){
        order.type = (*items).type;
        std::this_thread::sleep_for(std::chrono::seconds((*items).avg_time));
        (*items).buffer.insert_order(order);
        std::cout << "Produced order: " << ++num_produced << " Order Type: " << (*items).type << std::endl;
    }
};

