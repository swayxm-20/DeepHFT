#pragma once
#include "order_book.hpp"
#include "lock_free_queue.hpp"
#include <unordered_map>
#include <thread>
#include <atomic>
#include <iostream>

class EngineShard {
public:
    LockFreeQueue<Order>* queue; // Main thread pushes here
    std::thread worker;
    std::atomic<bool> running;
    int core_id;

    EngineShard(int id) : core_id(id), running(true) {
        queue = new LockFreeQueue<Order>(100000); // 100k buffer size
    }

    void start() {
        worker = std::thread(&EngineShard::loop, this);
    }

    void stop() {
        running = false;
        if (worker.joinable()) worker.join();
    }

private:
    // Each shard manages multiple symbols (e.g., Symbol 1, 5, 9...)
    std::unordered_map<uint32_t, OrderBook> books_;

    void loop() {
        std::cout << "[Core " << core_id << "] Shard Online.\n";
        
        while (running) {
            auto order_opt = queue->pop();
            if (order_opt) {
                Order o = *order_opt;
                // Find correct book and match
                auto trades = books_[o.symbol_id].match_order(o);
                
                // In production, we would send these trades to a network gateway
                // For demo, we just simulate work
            } else {
                // Busy wait for low latency (or yield to save battery)
                std::this_thread::yield();
            }
        }
    }
};