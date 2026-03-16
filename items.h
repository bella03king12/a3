#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <queue>


struct Item {
    int n;
    int avg_time;
    std::string type;
    std::queue<int> buffer;
};

struct Item_Main {
    int n;
    int avg_spot;
    int avg_market;
    int avg_ethexec;
    int avg_solexec;
    int avg_settler;
};


#endif