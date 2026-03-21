#include "consumer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"

/*
Stage 2 :
- Two executor threads (EthExec and SolExec) consume from the reserved order queue, simulate on-chain execution, and produce execution proofs in the execution queue.
- One settler thread consumes from the execution queue and simulates settlement finalization.
- Executors and settler print logs indicating their activity and the type of orders they are processing.
*/


// Executor thread: consume from reserved queue and produce execution proof in execution queue.
void *exec_consumer(void *arg) {
    ExecutorItem *items = (ExecutorItem *)arg;
    //int consumed = 0;

    std::cout << "Executor " << order_consumerNames[items->chain] << " started\n";

    while (items->reserved_queue->consume_counter < items->n) {
        // Wait for a reserved order to be available
        Order order = items->reserved_queue->remove_order();

        // Simulate on-chain execution delay
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));

        // Publish execution proof for settlement
        items->execution_queue->insert_order(order);
        //consumed++;

        std::cout << "Executor " << order_consumerNames[items->chain]
                  << " handled " << items->reserved_queue->consume_counter << " order(s) of type "
                  << order_producerNames[order.type] << "\n";
    }

    std::cout << "Executor " << order_consumerNames[items->chain] << " finished\n";
    return nullptr;
}
