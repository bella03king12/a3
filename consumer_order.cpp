#include "consumer_order.h"
#include "items.h"


void * consumer(void * arg){
    std::cout << "Start Consumer" << std::endl;
    Item * items = (Item *) arg;
    int num_consumed = 0;
    Order order;
    while (num_consumed < (*items).n){
        order = ((*items).buffer.remove_order());
        std::this_thread::sleep_for(std::chrono::seconds((*items).avg_time));
        std::cout << "Consuming order: " << ++num_consumed << " Order Type: " << (*items).type << std::endl;
    }
    std::cout << "End Consumer" << std::endl;
};