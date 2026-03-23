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

extern std::atomic<int> produced_spot_main;
extern std::atomic<int> produced_swap_main;
//extern std::atomic<int> produced_claimed;
extern std::chrono::high_resolution_clock::time_point start_time;

void *producer(void *arg) {
    ProducerItem *items = (ProducerItem *)arg;
    
    while (items->reserved_queue->order_counter < items->n) {
        //std::cout << "ITERATION: " << items->reserved_queue->order_counter << std::endl;
        //CHANGED
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));
        Order order{items->order_type};
        items->reserved_queue->insert_order(order);

        //std::cout << "PRODUCED: " << produced_claimed << " SPOT: " << produced_spot << " SWAP: " << produced_swap << std::endl;

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
        //std::cout << "Insert " << order.type << "Produced: " << items->reserved_queue->order_counter << std::endl;
    }
    produced_spot_main.store(items->reserved_queue->produced_spot.load());
    produced_swap_main.store(items->reserved_queue->produced_swap.load());
    return nullptr;
}