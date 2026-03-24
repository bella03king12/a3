// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#include "consumer_execution.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"
#include <semaphore.h> //CHANGED
#include "log.h"

// We need access to the global start time set in main for timing purposes
// Used for printing, not synchonization
extern std::chrono::high_resolution_clock::time_point start_time;

/*
 * Stage 3 - Settlement
 * --------------------
 * The settler thread is the last stop in the pipeline. It pulls completed
 * execution proofs out of the execution queue and finalizes each trade
 * (simulating on-chain settlement). There is only one settler thread, so
 * it simply counts how many proofs it has processed itself instead of
 * relying on a shared counter.
 */

// Settler thread: drain execution proof queue and finalize settlement records.
void *settler(void *arg) {
    SettlerItem *items = (SettlerItem *)arg;

    // Track how many proofs this settler has personally finished
    int settled = 0;

    // Keep going until we've settled every expected proof
    while (settled < items->n) {
        // Wait for an execution proof to settle
        Order order = items->execution_queue->remove_order();

        // Simulate settlement delay
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));

        settled++;

        // Build a label like "EthExecSPOT" or "SolExecSWAP" to describe this proof
        std::string proof_type = std::string(order_consumerNames[order.chain]) + order_producerNames[order.type];
        int queue_size = items->execution_queue->size();

        // Log that we just settled a proof (shows type, remaining queue size, total settled so far)
        log_removed_execution(
            proof_type.c_str(),
            static_cast<unsigned int>(queue_size),
            static_cast<unsigned int>(settled));
    }

    //signals main thread that every execution proof has been consumed and it is time to finish the program
    sem_post(items->semaphore);
    return nullptr;
}
