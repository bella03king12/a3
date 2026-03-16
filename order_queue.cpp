#include "order_queue.h"
#include "trade_pipeline.h"
#include "producer_order.h"
#include <vector>
#include <queue>


order_queue::order_queue(){
    // set to default values
    isAvailable = false;
    islocked = false;
    int num_orders = 25;
    int num_market_orders = 10;
    int max_orders = 25;
    int max_market_orders = 10;
};

void insert_order(OrderType type, std::vector<producer_order>){
    bool only_item;

    //lock.aquire();

    // is buffer full?
    if (buffer.size() == 0){
        
    }
};