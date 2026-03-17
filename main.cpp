#include <iostream>
#include "log.h"
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include "producer_order.h"
#include "consumer_order.h"
#include "items.h"
#include "order_queue.h"



int main(int argc, char *argv[]){

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


    // Parse through arguements

    //Double check that there is one number for each tag
    if ((argc - 1) % 2 != 0) {
        std::cout << "Error: Not enough arguements for tags." << std::endl;
        return 0;
    }

    //Parse the tags + if there isn't a tag, keep the default
    for (int i = 1; i < argc; i++){
        std::string value = argv[i];
        if (value[0] == '-'){
            switch(argv[i][1]) {
                case('n'):
                    n = std::stoi(argv[i + 1]);
                    break;
                case('s'):
                    avg_spot = std::stoi(argv[i + 1]);
                    break;
                case('w'):
                    avg_market = std::stoi(argv[i + 1]);
                    break;
                case('e'):
                    avg_ethexec = std::stoi(argv[i + 1]);
                    break;
                case('l'):
                    avg_solexec = std::stoi(argv[i + 1]);
                    break;
                case('t'):
                    avg_settler = std::stoi(argv[i + 1]);
                    break;
            }
        }
    }

    /* _____________________________________________
        SET UP MAIN THREAD
       _____________________________________________*/

    // Create the main thread
    // Create all producer and consumer threads
    // Terminate when consumer thread ends and signals completion
    // Use a barrier semaphore
    // Create shared data structure


    std::cout << "Start Main Thread" <<std::endl;

    // Note:
    // 1 is spot limit orders, 2 is market swaps, 3 is ethereum execution
    // 4 is solana execution, 5 settlement thread

    //set up thread info variables
    pthread_t T1, T3;
    int var1, var3;

    // create fake buffers for init testing
    order_queue queue_1 = order_queue(n);

    // Add all arguments into a struct
    Item product;
    Item consume;

    product.n = n;
    product.avg_time = avg_spot;
    product.type = "SPOT";
    product.buffer = queue_1;

    consume.n = n;
    consume.avg_time = avg_ethexec;
    consume.type = "ETHEXEC";
    consume.buffer = queue_1;

    // create semaphore to lock different threads
    sem_t lock;
    sem_init(&lock, 0, 1);


    // create threads with semaphore lock
    sem_wait(&lock);
    var1 = pthread_create(&T1, NULL, producer, &product);
    var3 = pthread_create(&T3, NULL, consumer, &consume);

    pthread_join(T1, NULL);
    pthread_join(T3, NULL);
    sem_post(&lock);
    

    std::cout << "End Main Thread" <<std::endl;


    return 0;
};