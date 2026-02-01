#pragma once
#include <cstdint>

enum class Side : uint8_t { Buy = 0, Sell = 1 };

// 64-Byte Aligned Struct for Cache Efficiency
struct alignas(64) Order {
    uint64_t id;
    uint64_t price;
    uint32_t quantity;
    uint32_t symbol_id; // Routing Key (e.g., 1=AAPL, 2=GOOGL)
    Side side;
    uint64_t timestamp;
    uint32_t padding;   // Explicit padding to fill 64 bytes
};

struct Trade {
    uint32_t symbol_id;
    uint64_t price;
    uint32_t quantity;
    uint64_t buyer_id;
    uint64_t seller_id;
};