#include "producer_order.h"
#include "items.h"
#include "order_queue.h"
#include "trade_pipeline.h"
#include <iostream>
#include <thread>
#include <chrono>

void *producer(void *arg) {
    ProducerItem *items = (ProducerItem *)arg;
    int produced = 0;

    std::cout << "Producer " << order_producerNames[items->order_type] << " started\n";

    while (produced < items->n) {
        std::this_thread::sleep_for(std::chrono::seconds(items->avg_time));
        Order order{items->order_type};
        items->reserved_queue->insert_order(order);
        produced++;
        std::cout << "Produced " << produced << " " << order_producerNames[order.type] << " order\n";
    }

    std::cout << "Producer " << order_producerNames[items->order_type] << " finished\n";
    return nullptr;
}
