#include "order_queue.h"
#include "trade_pipeline.h"
#include "producer_order.h"
#include "items.h"

order_queue::order_queue(int n){
    max = n;
    num_produced_total = 0;
    //num_produced_market = 0; ADD: multiple types later
    num_consumed = 0;
    lock = PTHREAD_MUTEX_INITIALIZER;
    cond_produce = PTHREAD_COND_INITIALIZER;
    cond_consume = PTHREAD_COND_INITIALIZER;
};


void order_queue::insert_order(Order order){
    bool only_item;

    pthread_mutex_lock(&lock);

    // check if space in the buffer
    if(buffer.size() == max){
        // make the calling thread wait till remove() consumer thread signals space is availiable
        pthread_cond_wait(&cond_produce, &lock);
    }

    //check if need to wake consumer
    only_item = buffer.empty();
    buffer.push(order);

    if(only_item){
        pthread_cond_signal(&cond_consume);
    }

    pthread_mutex_unlock(&lock);

};

Order order_queue::remove_order(){
    Order order;
    bool isFull;

    pthread_mutex_lock(&lock);

    if (buffer.empty()){
        // make the calling thread wait till insert() producer thread signals unconsumed
        pthread_cond_wait(&cond_consume, &lock);
    }

    //check if need to wake producer
    isFull = buffer.size() == max;

    //get front order from queue
    order = buffer.front();
    //remove saved order from queue
    buffer.pop();

    if (isFull){
        pthread_cond_signal(&cond_produce);
    }

    pthread_mutex_unlock(&lock);

};


