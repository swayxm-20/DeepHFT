#include "../include/order_book.hpp"

void OrderBook::add_order(const Order& order) {
    if (order.side == Side::Buy) {
        bids_[order.price].push_back(order);
    } else {
        asks_[order.price].push_back(order);
    }
}

std::vector<Trade> OrderBook::match_order(Order& incoming) {
    std::vector<Trade> trades;

    if (incoming.side == Side::Buy) {
        while (incoming.quantity > 0 && !asks_.empty()) {
            auto best_ask = asks_.begin();
            if (incoming.price < best_ask->first) break;

            auto& orders = best_ask->second;
            while (incoming.quantity > 0 && !orders.empty()) {
                Order& book_order = orders.front();
                uint32_t qty = std::min(incoming.quantity, book_order.quantity);

                trades.push_back({incoming.symbol_id, best_ask->first, qty, incoming.id, book_order.id});
                
                incoming.quantity -= qty;
                book_order.quantity -= qty;

                if (book_order.quantity == 0) orders.pop_front();
            }
            if (orders.empty()) asks_.erase(best_ask);
        }
    } else {
        while (incoming.quantity > 0 && !bids_.empty()) {
            auto best_bid = bids_.begin();
            if (incoming.price > best_bid->first) break;

            auto& orders = best_bid->second;
            while (incoming.quantity > 0 && !orders.empty()) {
                Order& book_order = orders.front();
                uint32_t qty = std::min(incoming.quantity, book_order.quantity);

                trades.push_back({incoming.symbol_id, best_bid->first, qty, book_order.id, incoming.id});
                
                incoming.quantity -= qty;
                book_order.quantity -= qty;

                if (book_order.quantity == 0) orders.pop_front();
            }
            if (orders.empty()) bids_.erase(best_bid);
        }
    }

    if (incoming.quantity > 0) add_order(incoming);
    return trades;
}