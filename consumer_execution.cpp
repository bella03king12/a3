#include "consumer_execution.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"
#include <semaphore.h>
/*
Stage 3 :
- Two executor threads (EthExec and SolExec) consume from the reserved order queue, simulate on-chain execution, and produce execution proofs in the execution queue.
- One settler thread consumes from the execution queue and simulates settlement finalization.
- Executors and settler print logs indicating their activity and the type of orders they are processing.
*/

// Settler thread: drain execution proof queue and finalize settlement records.
void *settler(void *arg) {
    SettlerItem *items = (SettlerItem *)arg;

    // there aren't multiple settler threads so it can count internally using settled instead of order_counter
    int settled = 0;

    std::cout << "Settler started\n";

    while (settled < items->n) {
        // Wait for an execution proof to settle
        Order order = items->execution_queue->remove_order();

        // Simulate settlement delay
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));

        settled++;
        std::cout << "Settler finalized " << settled << " proof(s) of type "
                  << order_producerNames[order.type] << "\n";
    }

    std::cout << "Settler finished\n";

    //CHANGED
    //signals main thread that every execution proof has been consumed and it is time to finish the program
    sem_post(items->semaphore);
    return nullptr;
}
