#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include <vector>
#include <queue>
#include <atomic>
#include <pthread.h>
#include <iostream>

class order_queue {
    public:
        // push() add order to back, pop() remove first order, empty() return bool, size() num of elements
        std::queue<Order> buffer;
        void insert_order(Order order);
        Order remove_order();
        order_queue(int n);

    private:
        pthread_mutex_t lock;
        pthread_cond_t cond_consume;
        pthread_cond_t cond_produce;
        int num_produced_total;
        //int num_produced_market; ADD: multiple types later
        int num_consumed;
        int max;
};

#endif