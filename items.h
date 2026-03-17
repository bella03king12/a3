#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <queue>
#include "order_queue.h"


// generic arguments for a producer and consumer
struct Item {
    int n;
    int avg_time;
    std::string type;
    order_queue buffer;
};

// SPOT, MARKET
struct Order{
    std::string type;
};


#endif