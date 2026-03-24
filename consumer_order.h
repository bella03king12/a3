// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#ifndef CONSUMER_ORDER_H
#define CONSUMER_ORDER_H

// exec_consumer() - One of the two executor threads (Eth or Sol).
// It loops until all n orders have been consumed across both executors,
// processes each order, and forwards an execution proof to the next stage.
// input ConsumerItem (technically void *)
// output void

void *exec_consumer(void *arg);

#endif
