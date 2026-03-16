#ifndef CONSUMER_ORDER_H
#define CONSUMER_ORDER_H

#include <string>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>


class consumer_order{
    public:
        void * consumer(void * arg);
};

#endif