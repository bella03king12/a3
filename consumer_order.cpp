// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#include "consumer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"
#include "log.h"

extern std::chrono::high_resolution_clock::time_point start_time;
extern std::atomic<int> execution_claimed;

/*
Stage 2 :
- Two executor threads (EthExec and SolExec) consume from the reserved order queue, simulate on-chain execution, and produce execution proofs in the execution queue.
- One settler thread consumes from the execution queue and simulates settlement finalization.
- Executors and settler print logs indicating their activity and the type of orders they are processing.
*/


// Executor thread: consume from reserved queue and produce execution proof in execution queue.
void *exec_consumer(void *arg) {
    ExecutorItem *items = (ExecutorItem *)arg;

    while (items->reserved_queue->consume_counter < items->n) {
        //CHANGED
        execution_claimed.fetch_add(1);
        // Wait for a reserved order to be available
        Order order = items->reserved_queue->remove_order();

        // Simulate on-chain execution delay
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));
    
        // CHANGED
        // Update consumed counts
        if (order.type == SpotLimit) {
            items->consumed_spot++;
        } else {
            items->consumed_swap++;
        }

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

        // Publish execution proof for settlement
        items->execution_queue->insert_order(order);

        std::string proof_type = std::string(order_consumerNames[items->chain]) + order_producerNames[order.type];
        int queue_size = items->execution_queue->size();
        log_added_execution(proof_type.c_str(), static_cast<unsigned int>(queue_size));
    }
    return nullptr;
}
