#include "producer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"
#include "log.h"

extern std::atomic<int> produced_spot;
extern std::atomic<int> produced_swap;
extern std::atomic<int> produced_claimed;
extern std::chrono::high_resolution_clock::time_point start_time;

void *producer(void *arg) {
    ProducerItem *items = (ProducerItem *)arg;

    while (items->reserved_queue->order_counter < items->n) {
        //CHANGED
        produced_claimed.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));
        Order order{items->order_type};
        items->reserved_queue->insert_order(order);

        //CHANGED
        if (order.type == SpotLimit) {
            produced_spot++;
        } else {
            produced_swap++;
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
    return nullptr;
}
