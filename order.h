// ISABELLA KING (129914717)
// JOSE HERNANDEZ SANCHEZ (826465400)

#ifndef ORDER_H
#define ORDER_H

#include "trade_pipeline.h"
/*
Purpose: This file defines the Order structure used in the trade pipeline. 
Each Order has a type, which is defined by the OrderType enum in trade_pipeline.h. 
The Order structure can be extended in the future to include additional fields such as 
order ID, quantity, price, etc., as needed for more complex order processing.
*/
struct Order {
    OrderType type;
    ExecChainType chain;  // Execution chain for proofs
};

#endif
