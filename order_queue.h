#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include <queue>
#include <pthread.h>
#include <iostream>
#include "order.h"
#include <atomic>
#include <semaphore.h>  // CHANGED: Added for semaphore-based synchronization instead of condition variables

class order_queue {
    public:
        order_queue(int n);
        ~order_queue();  // CHANGED: Added destructor to clean up semaphores
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
        std::queue<Order> buffer;

    private:
        pthread_mutex_t lock;
        sem_t empty_slots;  // CHANGED: Semaphore for available empty slots in queue
        sem_t full_slots;   // CHANGED: Semaphore for available full slots (items) in queue
        sem_t market_swap_slots;  // CHANGED: Semaphore for limiting MarketSwap orders to 10
        int max;
        int market_swap_in_queue;
        int spot_in_queue;
        int swap_in_queue;
};

#endif
