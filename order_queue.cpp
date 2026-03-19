#include "order_queue.h"

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
    lock = PTHREAD_MUTEX_INITIALIZER;
    cond_produce = PTHREAD_COND_INITIALIZER;
    cond_consume = PTHREAD_COND_INITIALIZER;
}

void order_queue::insert_order(Order order) {
    pthread_mutex_lock(&lock);

    // check for space in the bugger based on number of orders and type of order

    while ((int)buffer.size() == max ||
           (order.type == MarketSwap && market_swap_in_queue >= 10)) {
        // make the calling thread wait till remove() consumer thread signals space is availiable
        pthread_cond_wait(&cond_produce, &lock);
    }

    //check if need to wake consumer
    bool was_empty = buffer.empty();
    buffer.push(order);


    //atomic int var that increments upon each order that is added
    //should be a safe increment (atomic and in critical section)
    order_counter++;


    if (order.type == MarketSwap) {
        market_swap_in_queue++;
    }

    if (was_empty) {
        pthread_cond_signal(&cond_consume);
    }

    pthread_mutex_unlock(&lock);
}

Order order_queue::remove_order() {
    pthread_mutex_lock(&lock);

    while (buffer.empty()) {
        // make the calling thread wait till insert() producer thread signals unconsumed
        pthread_cond_wait(&cond_consume, &lock);
    }

    //check if need to wake producer
    bool was_full = ((int)buffer.size() == max);

    //get front order from queue
    Order order = buffer.front();
    //remove saved order from queue
    buffer.pop();
    consume_counter++;
    if (order.type == MarketSwap) {
        market_swap_in_queue--;
    }

    if (was_full || (order.type == MarketSwap)) {
        pthread_cond_signal(&cond_produce);
    }

    pthread_mutex_unlock(&lock);
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
