#ifndef ITEMS_H
#define ITEMS_H

#include "order_queue.h"
#include "trade_pipeline.h"

/*
This file defines the data structures for producer, executor, and settler threads in the trade pipeline.
- ProducerItem: Contains parameters for producer threads, including the number of orders to produce, average time per order, order type, and reference to the reserved queue.
- ExecutorItem: Contains parameters for executor threads, including the number of orders to consume, average time per execution, execution chain type, and references to both the reserved queue and execution queue.
- SettlerItem: Contains parameters for the settler thread, including the number of execution proofs to settle, average time per settlement, and reference to the execution queue.
*/

struct ProducerItem {
    int n;
    int avg_time;
    OrderType order_type;
    order_queue* reserved_queue;
};

struct ExecutorItem {
    int n;
    int avg_time;
    ExecChainType chain;
    order_queue* reserved_queue;
    order_queue* execution_queue;
    int consumed_spot;
    int consumed_swap;
};

struct SettlerItem {
    int n;
    int avg_time;
    order_queue* execution_queue;
};

#endif
