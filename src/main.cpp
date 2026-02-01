#include <iostream>
#include <vector>
#include <chrono>
#include "../include/engine_shard.hpp"

// CONFIG
const int NUM_CORES = 4;
const int ORDERS_TO_SEND = 1000000;

int main() {
    std::cout << "=== DeepHFT: Multi-Core Sharded Exchange ===\n";

    // 1. Initialize Shards (Worker Threads)
    std::vector<EngineShard*> shards;
    for (int i = 0; i < NUM_CORES; ++i) {
        auto* shard = new EngineShard(i);
        shards.push_back(shard);
        shard->start();
    }

    std::cout << "[System] " << NUM_CORES << " Cores Running. Starting Traffic...\n";
    auto start = std::chrono::high_resolution_clock::now();

    // 2. Traffic Injection Loop (Simulating Network)
    for (int i = 0; i < ORDERS_TO_SEND; ++i) {
        Order o;
        o.id = i;
        o.price = 100 + (i % 20);
        o.quantity = 10;
        o.symbol_id = i % 100; // 100 different stocks
        o.side = (i % 2 == 0) ? Side::Buy : Side::Sell;

        // ROUTING LOGIC: Determine which core owns this symbol
        int target_shard = o.symbol_id % NUM_CORES;

        // Push to that core's queue
        while (!shards[target_shard]->queue->push(o)) {
            // If queue is full, spin-wait (Backpressure)
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "[System] Dispatched " << ORDERS_TO_SEND << " orders in " << duration.count() << "ms.\n";

    // 3. Cleanup
    std::cout << "[System] Shutting down cores...\n";
    for (auto* shard : shards) {
        shard->stop();
        delete shard;
    }

    return 0;
}