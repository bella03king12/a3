#include "order_queue.h"

/*
Stage 1:
- Implement a thread-safe order queue with a maximum capacity of `n` orders.
- The queue should support two operations: `insert_order` for producers to add orders and `
remove_order` for consumers to retrieve orders.
- The queue should block producers when it is full and block consumers when it is empty.
- Additionally, the queue should enforce a limit of 10 MarketSwap orders at any time to
prevent overloading the execution stage with too many complex orders.
*/
order_queue::order_queue(int n) {
    max = n;
    market_swap_in_queue = 0;
    lock = PTHREAD_MUTEX_INITIALIZER;
    cond_produce = PTHREAD_COND_INITIALIZER;
    cond_consume = PTHREAD_COND_INITIALIZER;
}

void order_queue::insert_order(Order order) {
    pthread_mutex_lock(&lock);

    while ((int)buffer.size() == max ||
           (order.type == MarketSwap && market_swap_in_queue >= 10)) {
        pthread_cond_wait(&cond_produce, &lock);
    }

    bool was_empty = buffer.empty();
    buffer.push(order);
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
        pthread_cond_wait(&cond_consume, &lock);
    }

    bool was_full = ((int)buffer.size() == max);
    Order order = buffer.front();
    buffer.pop();
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
