#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <queue>


// generic arguments for a producer and consumer
struct Item {
    int n;
    int avg_time;
    std::string type;
    std::queue<int> buffer;
};

// SPOT, MARKET
struct Order{
    std::string type;
};


#endif