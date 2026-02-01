#pragma once
#include "types.hpp"
#include <map>
#include <list>
#include <vector>

class OrderBook {
public:
    void add_order(const Order& order);
    std::vector<Trade> match_order(Order& incoming);

private:
    // Bids: Highest price first (std::greater)
    std::map<uint64_t, std::list<Order>, std::greater<uint64_t>> bids_;
    
    // Asks: Lowest price first (std::less)
    std::map<uint64_t, std::list<Order>> asks_;
};