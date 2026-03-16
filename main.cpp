#include <iostream>
#include "log.h"
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "producer_order.h"
#include "consumer_order.h"
#include "items.h"

// Create the main thread
// Create all producer and consumer threads
// Terminate when consumer thread ends and signals completion
// Use a barrier semaphore
// Create shared data structure

void * main_thread(void * arg) {
        pthread_t T1, T3;
        int var1, var3;
        Item_Main * items_main = (Item_Main *) arg;

        std::queue<int> buffer1;
        std::queue<int> buffer2;

        Item product;
        Item consume;

        product.n = (*items_main).n;
        product.avg_time = (*items_main).avg_spot;
        product.type = "SPOT";
        product.buffer = buffer1;

        consume.n = (*items_main).n;
        consume.avg_time = (*items_main).avg_ethexec;
        consume.type = "ETHEXEC";
        consume.buffer = buffer2;

        //need objects to call the functions???
        producer_order caller1;
        consumer_order caller2;

        // IDEAS:
        // (producer_order *) producer
        // caller1.producer()
        // IDK :(


        var1 = pthread_create(&T1, NULL, producer, (void *) &product);
        var3 = pthread_create(&T3, NULL, consumer, (void *) &consume);

        return NULL;
};

int main(int argc, char *argv[]){
    std::cout << "Start Argument Setup" <<std::endl;

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

    //Parse the tags
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

    std::cout << "Finish Argument Setup" <<std::endl;

    /* _____________________________________________
        SET UP MAIN THREAD
       _____________________________________________*/

    std::cout << "Start Main Thread" <<std::endl;

    pthread_t T_main;
    int var;
    Item_Main items_main;

    items_main.n = n;
    items_main.avg_spot = avg_spot;
    items_main.avg_market = avg_market;
    items_main.avg_ethexec = avg_ethexec;
    items_main.avg_solexec = avg_solexec;
    items_main.avg_settler = avg_settler;

    var = pthread_create(&T_main, NULL, main_thread, (void*) &items_main);

    std::cout << "End Main Thread" <<std::endl;

    return 0;
};