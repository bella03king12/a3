#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

class order_queue {
    public:
        // inserts is used by producers to add items to the shared buffer
        void insert_order(OrderType type, std::vector<producer_order>);
        void remove_order(OrderType type, std::vector<producer_order>);

    private:
        bool isAvailable;
        std::queue<producer_order> buffer;
        int num_orders;
        int num_market_orders;
        int max_orders;
        int max_market_orders;
        bool islocked;

};

#endif