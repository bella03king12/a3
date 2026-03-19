#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include <queue>
#include <pthread.h>
#include <iostream>
#include "order.h"
#include <atomic>

class order_queue {
    public:
        order_queue(int n);
        void insert_order(Order order);
        Order remove_order();
        int size();
        bool empty();
        //shared var so should use atomic<>. 
        //Other vars are only updated or read in the critical section 
        //and are not shared with producer and consumer
        std::atomic<int> order_counter{0};
        std::atomic<int> consume_counter{0};
        std::queue<Order> buffer;

    private:
        pthread_mutex_t lock;
        pthread_cond_t cond_consume;
        pthread_cond_t cond_produce;
        int max;
        int market_swap_in_queue;
};

#endif
