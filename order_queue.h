// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include <queue>
#include <pthread.h>
#include <iostream>
#include "order.h"
#include <atomic>

class order_queue {
    public:
        // Set up the queue with a maximum size and initialize all counters and sync primitives
        order_queue(int n, int production_limit);

        // insert_order() - Called by a producer thread to add an order to the queue.
        // Blocks the producer if the queue is full, or if it's a MarketSwap and
        // there are already 10 MarketSwaps waiting (to prevent overloading execution).

        void insert_order(Order order);

        // remove_order() - Called by a consumer thread to take the next order.
        // Blocks the consumer if the queue is empty and waits for a producer to add something.

        Order remove_order();

        // size() - Returns how many orders are currently in the queue (thread-safe snapshot)

        int size();

        // empty() - Returns true if the queue has no orders right now (thread-safe snapshot)

        bool empty();

        // Getters for queue counters

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

        // shared buffer between producer and consumer

        std::queue<Order> buffer;

    private:
        // lock and conditional variables
        pthread_mutex_t lock;
        pthread_cond_t cond_consume;
        pthread_cond_t cond_produce;
        // The most orders we can hold at once in the buffer
        int max;
        int market_swap_in_queue;
        int spot_in_queue;
        int swap_in_queue;
        int production_max;
};

#endif
