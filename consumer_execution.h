// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#ifndef CONSUMER_EXECUTION_H
#define CONSUMER_EXECUTION_H


// Settler thread: drain execution proof queue and finalize settlement records.
// input: SettlerItem (technically void *)
// output void

void *settler(void *arg);

#endif
