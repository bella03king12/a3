// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#ifndef PRODUCER_ORDER_H
#define PRODUCER_ORDER_H


// producer() - Generates orders of a single type until n total orders have
// been placed in the reserved queue. Both producer threads share the same
// order_counter, so together they stop once n total orders are produced.
// input: ProducerItem (technically void *)
// output: void *

void *producer(void *arg);

#endif
