#include "order_queue.h"
#include <semaphore.h>  // CHANGED: Added for semaphore operations

/*
Stage 1:
- Implement a thread-safe order queue with a maximum capacity of `n` orders.
- The queue should support two operations: `insert_order` for producers to add orders and `
remove_order` for consumers to retrieve orders.
- The queue should block producers when it is full and block consumers when it is empty.
- Additionally, the queue should enforce a limit of 10 MarketSwap orders at any time to
prevent overloading the execution stage with too many complex orders.
- The reserved queue can hold 25 orders of any type max and the execution queue can
  hold 15 executions proofs of any type max.
*/


order_queue::order_queue(int n) {
    // max size of the buffer
    max = n;
    // number of market_swap types in buffer
    market_swap_in_queue = 0;
    spot_in_queue = 0;
    swap_in_queue = 0;
    lock = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&empty_slots, 0, max);  // CHANGED: Initialize semaphore for empty slots to max
    sem_init(&full_slots, 0, 0);     // CHANGED: Initialize semaphore for full slots to 0
    sem_init(&market_swap_slots, 0, 10);  // CHANGED: Initialize semaphore for MarketSwap limit to 10
}

order_queue::~order_queue() {  // CHANGED: Destructor to destroy semaphores
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&market_swap_slots);
}

void order_queue::insert_order(Order order) {
    if (order.type == MarketSwap) {
        sem_wait(&market_swap_slots);  // CHANGED: Wait for available MarketSwap slot before proceeding
    }
    sem_wait(&empty_slots);  // CHANGED: Wait for empty slot in queue
    pthread_mutex_lock(&lock);

    buffer.push(order);
    order_counter++;
    if (order.type == MarketSwap) {
        market_swap_in_queue++;
        swap_in_queue++;
    } else {
        spot_in_queue++;
    }

    pthread_mutex_unlock(&lock);
    sem_post(&full_slots);  // CHANGED: Signal that a full slot is now available
}

Order order_queue::remove_order() {
    sem_wait(&full_slots);  // CHANGED: Wait for available item in queue
    pthread_mutex_lock(&lock);

    Order order = buffer.front();
    buffer.pop();
    consume_counter++;
    if (order.type == MarketSwap) {
        market_swap_in_queue--;
        swap_in_queue--;
        sem_post(&market_swap_slots);  // CHANGED: Release MarketSwap slot
    } else {
        spot_in_queue--;
    }

    pthread_mutex_unlock(&lock);
    sem_post(&empty_slots);  // CHANGED: Signal that an empty slot is now available
    return order;
}

int order_queue::size() {
    pthread_mutex_lock(&lock);
    int s = buffer.size();
    pthread_mutex_unlock(&lock);
    return s;
}

bool order_queue::empty() {
    pthread_mutex_lock(&lock);
    bool e = buffer.empty();
    pthread_mutex_unlock(&lock);
    return e;
}
