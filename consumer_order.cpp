#include "consumer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"
#include "log.h"

// Global start time (set in main) used for timing/logging
extern std::chrono::high_resolution_clock::time_point start_time;
// Shared counter that tracks how many executor threads have claimed work this loop cycle
extern std::atomic<int> execution_claimed;

/*
 * Stage 2 - Execution
 * -------------------
 * There are two executor threads: EthExec (Ethereum) and SolExec (Solana).
 * Each one pulls reserved orders off the reserved queue, simulates executing
 * that trade on its blockchain, then pushes an execution proof into the
 * execution queue for the settler to finalize.
 */

// exec_consumer() - One of the two executor threads (Eth or Sol).
// It loops until all n orders have been consumed across both executors,
// processes each order, and forwards an execution proof to the next stage.
void *exec_consumer(void *arg) {
    ExecutorItem *items = (ExecutorItem *)arg;

    // Keep looping as long as there are still orders that haven't been consumed yet
    while (items->reserved_queue->consume_counter < items->n) {
        // Announce that this executor is about to claim an order
        execution_claimed.fetch_add(1);
        // Wait for a reserved order to be available
        Order order = items->reserved_queue->remove_order();

        // Simulate on-chain execution delay
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));

        // Keep a local count of how many of each type this executor has processed
        if (order.type == SpotLimit) {
            items->consumed_spot++;
        } else {
            items->consumed_swap++;
        }

        // Build snapshot arrays for the log (how many consumed + how many still in queue)
        unsigned int consumed[OrderTypeN] = {
            static_cast<unsigned int>(items->consumed_spot),
            static_cast<unsigned int>(items->consumed_swap)
        };
        unsigned int in_queue[OrderTypeN] = {
            static_cast<unsigned int>(items->reserved_queue->get_spot_in_queue()),
            static_cast<unsigned int>(items->reserved_queue->get_swap_in_queue())
        };
        OrderRemoved removed = {items->chain, order.type, consumed, in_queue};
        log_removed_order(removed);

        // Set chain for proof  // CHANGED: Assign execution chain to order for proof identification
        order.chain = items->chain;

        // Push the execution proof into the execution queue for the settler
        items->execution_queue->insert_order(order);

        // Log that we added an execution proof (e.g. "EthExecSPOT")
        std::string proof_type = std::string(order_consumerNames[items->chain]) + order_producerNames[order.type];
        int queue_size = items->execution_queue->size();
        log_added_execution(proof_type.c_str(), static_cast<unsigned int>(queue_size));
    }
    return nullptr;
}
