#include <iostream>
#include <string>
#include <pthread.h>
#include "order_queue.h"
#include "producer_order.h"
#include "consumer_order.h"
#include "consumer_execution.h"
#include "items.h"
#include "trade_pipeline.h"
#include <semaphore.h>



int main(int argc, char *argv[]) {

    /* __________________
        SET UP ARGUEMENTS
       __________________*/

    
    // Set defaults arguements
    int n = 120;
    int avg_spot = 0;
    int avg_market = 0;
    int avg_ethexec = 0;
    int avg_solexec = 0;
    int avg_settler = 0;


    //Double check that there is one number for each tag    
    if ((argc - 1) % 2 != 0) {
        std::cout << "Error: Not enough arguments for tags." << std::endl;
        return 1;
    }

    //Parse the tags + if there isn't a tag, keep the default
    for (int i = 1; i < argc; i += 2) {
        std::string value = argv[i];
        if (value[0] == '-') {
            switch (argv[i][1]) {
                case 'n': n = std::stoi(argv[i + 1]); break;
                case 's': avg_spot = std::stoi(argv[i + 1]); break;
                case 'w': avg_market = std::stoi(argv[i + 1]); break;
                case 'e': avg_ethexec = std::stoi(argv[i + 1]); break;
                case 'l': avg_solexec = std::stoi(argv[i + 1]); break;
                case 't': avg_settler = std::stoi(argv[i + 1]); break;
            }
        }
    }

    

    /* ____________
        MAIN THREAD
       ____________*/


    std::cout << "Start Main Thread" << std::endl;

    //CHANGED
    // Get semaphore ready
    sem_t barrier;
    sem_init(&barrier, 0, 0);

    // Queues: reserved orders for execution and execution proofs for settlement
    order_queue reserved_queue(25);
    order_queue execution_queue(15);

    // Two producers, one for each order type
    ProducerItem p1 = {n, avg_spot, SpotLimit, &reserved_queue};
    ProducerItem p2 = {n, avg_market, MarketSwap, &reserved_queue};

    // Two executor threads: Eth and Sol
    ExecutorItem e1 = {n, avg_ethexec, EthExec, &reserved_queue, &execution_queue};
    ExecutorItem e2 = {n, avg_solexec, SolExec, &reserved_queue, &execution_queue};

    // One settler that settles both execution proofs
    SettlerItem s = {n, avg_settler, &execution_queue, &barrier};

    pthread_t t_p1, t_p2, t_e1, t_e2, t_s;


    int r1 = pthread_create(&t_p1, NULL, producer, &p1);
    int r2 = pthread_create(&t_p2, NULL, producer, &p2);
    int r3 = pthread_create(&t_e1, NULL, exec_consumer, &e1);
    int r4 = pthread_create(&t_e2, NULL, exec_consumer, &e2);
    int r5 = pthread_create(&t_s, NULL, settler, &s);

    //CHANGED
    // checked: threads made successfully
    if (r1 || r2 || r3 || r4 || r5) {
        std::cerr << "Error: failed to create one or more threads." << std::endl;
        return 1;
    }

    //CHANGED
    // Makes the main thread sleep till t_s signals that it has finished
    sem_wait(&barrier);

    std::cout << "End Main Thread" << std::endl;
    return 0;
}
