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
 /*
  Thread-safe order queue
  This queue sits between the producers and the consumers. It has a max
  capacity and uses a mutex + condition variables to safely block threads
  when needed:
    - Producers block when the queue is full (or when there are already
      10 MarketSwap orders waiting, to avoid overloading execution).
    - Consumers block when the queue is empty.
  
  The reserved queue holds up to 25 orders; the execution queue holds up to 15.
 */


// Set up the queue with a maximum size and initialize all counters and sync primitives


order_queue::order_queue(int n) {
    // The most orders we can hold at once
    max = n;
    // Tracks how many MarketSwap orders are currently in the queue (capped at 10)
    market_swap_in_queue = 0;
    // Separate counters so we can quickly report how many of each type are waiting
    spot_in_queue = 0;
    swap_in_queue = 0;
    lock = PTHREAD_MUTEX_INITIALIZER;
    cond_produce = PTHREAD_COND_INITIALIZER;
    cond_consume = PTHREAD_COND_INITIALIZER;
}

// insert_order() - Called by a producer thread to add an order to the queue.
// Blocks the producer if the queue is full, or if it's a MarketSwap and
// there are already 10 MarketSwaps waiting (to prevent overloading execution).
void order_queue::insert_order(Order order) {
    pthread_mutex_lock(&lock);

    // Wait here if the queue is full, or if we already have 10 MarketSwap orders
    while ((int)buffer.size() >= max ||
           (order.type == MarketSwap && market_swap_in_queue >= 10)) {
        // Sleep until a consumer removes something and signals us to wake up
        pthread_cond_wait(&cond_produce, &lock);
    }

    bool was_empty = buffer.empty();
    buffer.push(order);

    // order_counter tracks the total number of orders ever inserted (used to know when to stop)
    // produced_claimed is also incremented here safely inside the critical section
    order_counter++;
    produced_claimed++;

    // Update type-specific counters so consumers and logs can see what's in the queue
    if (order.type == MarketSwap) {
        market_swap_in_queue++;
        swap_in_queue++;
        produced_swap++;
    } else {
        spot_in_queue++;
        produced_spot++;
    }

    // Wake up a waiting consumer now that there's something in the queue
    // if (was_empty) {
    //    pthread_cond_signal(&cond_consume);
    // }

    pthread_mutex_unlock(&lock);
}

// remove_order() - Called by a consumer thread to take the next order.
// Blocks the consumer if the queue is empty and waits for a producer to add something.
Order order_queue::remove_order() {
    pthread_mutex_lock(&lock);

    // Wait here if there's nothing to consume
    while (buffer.empty()) {
        // Sleep until a producer adds an order and signals us to wake up
        pthread_cond_wait(&cond_consume, &lock);
    }

    bool was_full = ((int)buffer.size() == max);

    // Grab the oldest order from the front of the queue (FIFO)
    Order order = buffer.front();
    buffer.pop();
    consume_counter++;

    // Keep the type counters in sync after removing
    if (order.type == MarketSwap) {
        market_swap_in_queue--;
        swap_in_queue--;
    } else {
        spot_in_queue--;
    }

    // Wake up a waiting producer now that there's space in the queue
    // if (was_full || (order.type == MarketSwap)) {
    //    pthread_cond_signal(&cond_produce);
    // }

    pthread_mutex_unlock(&lock);
    return order;
}

// size() - Returns how many orders are currently in the queue (thread-safe snapshot)
int order_queue::size() {
    pthread_mutex_lock(&lock);
    int s = buffer.size();
    pthread_mutex_unlock(&lock);
    return s;
}

// empty() - Returns true if the queue has no orders right now (thread-safe snapshot)
bool order_queue::empty() {
    pthread_mutex_lock(&lock);
    bool e = buffer.empty();
    pthread_mutex_unlock(&lock);
    return e;
}
