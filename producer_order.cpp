#include "producer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "log.h"
#include "order.h"

extern std::atomic<int> produced_spot;
extern std::atomic<int> produced_swap;
extern std::atomic<int> produced_claimed;
extern std::chrono::high_resolution_clock::time_point start_time;

void *producer(void *arg) {
    ProducerItem *items = (ProducerItem *)arg;

    std::cout << "Producer " << order_producerNames[items->order_type] << " started\n";

    while (true) {
        int ticket = produced_claimed.fetch_add(1);
        if (ticket >= items->n) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));
        Order order{items->order_type};
        items->reserved_queue->insert_order(order);
        if (order.type == SpotLimit) {
            produced_spot++;  // CHANGED: Increment global produced spot count
        } else {
            produced_swap++;  // CHANGED: Increment global produced swap count
        }
        unsigned int produced[OrderTypeN] = {
            static_cast<unsigned int>(produced_spot.load()),
            static_cast<unsigned int>(produced_swap.load())
        };
        unsigned int in_queue[OrderTypeN] = {
            static_cast<unsigned int>(items->reserved_queue->get_spot_in_queue()),
            static_cast<unsigned int>(items->reserved_queue->get_swap_in_queue())
        };
        OrderAdded added = {order.type, produced, in_queue};
        log_added_order(added);
    }

    std::cout << "Producer " << order_producerNames[items->order_type] << " finished\n";
    return nullptr;
}
