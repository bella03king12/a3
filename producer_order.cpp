#include "producer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "order.h"

void *producer(void *arg) {
    ProducerItem *items = (ProducerItem *)arg;
    //int produced = 0;

    std::cout << "Producer " << order_producerNames[items->order_type] << " started\n";

    while (items->reserved_queue->order_counter < items->n) {
        std::this_thread::sleep_for(std::chrono::milliseconds(items->avg_time));
        Order order{items->order_type};
        items->reserved_queue->insert_order(order);
        //produced++;
        std::cout << "Produced " << items->reserved_queue->order_counter << " " << order_producerNames[order.type] << " order\n";
    }

    std::cout << "Producer " << order_producerNames[items->order_type] << " finished\n";
    return nullptr;
}
