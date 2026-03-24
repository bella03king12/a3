// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#include "producer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"
#include "log.h"

// These globals are written by the producers when they finish so main can log the final totals
// Used for printing, not synchonization
extern std::atomic<int> produced_spot_main;
extern std::atomic<int> produced_swap_main;
extern std::chrono::high_resolution_clock::time_point start_time;

/*
 * Stage 1 - Order Production
 * --------------------------
 * There are two producer threads: one that creates SpotLimit orders and one
 * that creates MarketSwap orders. Each producer runs this same function.
 * It generates one order at a time, waits a bit between each one (simulating
 * real-world order arrival rate), and pushes orders into the reserved queue.
 */

// producer() - Generates orders of a single type until n total orders have
// been placed in the reserved queue. Both producer threads share the same
// order_counter, so together they stop once n total orders are produced.

void *producer(void *arg) {
    ProducerItem *items = (ProducerItem *)arg;

    // Keep producing until the shared counter reaches n total orders
    while (items->reserved_queue->order_counter < items->n) {

        // simulates production time
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));

        // Create a new order of this producer's assigned type
        Order order{items->order_type};

        // Push the order into the reserved queue (may block if the queue is full)
        items->reserved_queue->insert_order(order);

        // Build snapshot arrays so the log can show how many have been produced
        // and how many are currently sitting in the queue
        unsigned int produced[OrderTypeN] = {
            static_cast<unsigned int>(items->reserved_queue->produced_spot.load()),
            static_cast<unsigned int>(items->reserved_queue->produced_swap.load())
        };
        unsigned int in_queue[OrderTypeN] = {
            static_cast<unsigned int>(items->reserved_queue->get_spot_in_queue()),
            static_cast<unsigned int>(items->reserved_queue->get_swap_in_queue())
        };
        OrderAdded added = {order.type, produced, in_queue};
        log_added_order(added);
    }

    // Save final totals into the global vars so main can print the summary
    produced_spot_main.store(items->reserved_queue->produced_spot.load());
    produced_swap_main.store(items->reserved_queue->produced_swap.load());
    return nullptr;
}