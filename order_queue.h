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
        // CHANGED: Added getters for queue counters
        int get_spot_in_queue() { return spot_in_queue; }
        int get_swap_in_queue() { return swap_in_queue; }

        //shared var so should use atomic<>. 
        //Other vars are only updated or read in the critical section 
        //and are not shared with producer and consumer
        std::atomic<int> order_counter{0};
        std::atomic<int> consume_counter{0};
        std::atomic<int> produced_spot{0};
        std::atomic<int> produced_swap{0};
        std::atomic<int> produced_claimed{0};
        std::queue<Order> buffer;

    private:
        pthread_mutex_t lock;
        pthread_cond_t cond_consume;
        pthread_cond_t cond_produce;
        int max;
        int market_swap_in_queue;
        // CHANGED 
        int spot_in_queue;
        int swap_in_queue;
};

#endif
