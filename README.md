# DeepHFT

![C++](https://img.shields.io/badge/C++-20-blue.svg?style=flat&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.15+-064F8C.svg?style=flat&logo=cmake)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A high-performance, lock-free sharded matching engine simulator designed for low-latency order processing in high-frequency trading scenarios.

---

## Overview

**DeepHFT** is a multi-threaded matching engine that demonstrates core systems engineering principles applied to financial trading infrastructure. The engine partitions order flow by ticker symbol across CPU cores, enabling parallel, lock-free order matching with deterministic Price-Time Priority execution.

This project showcases:
- **Lock-free inter-core communication** using custom SPSC ring buffers
- **Cache-aware data structures** to minimize false sharing and maximize throughput
- **Deterministic sharding** for predictable latency and horizontal scalability

Built as a portfolio piece for systems programming and HFT infrastructure roles.

---

## Key Engineering Concepts

### 1. **Ticker Sharding for Parallelism**
Instead of a single global order book protected by locks, DeepHFT partitions symbols across CPU cores using deterministic hashing (`symbol_id % core_count`). Each core owns a disjoint subset of order books, eliminating contention and enabling true parallel execution.

**Why it matters:** Lock contention is the enemy of low latency. By sharding at the symbol level, we achieve O(1) routing and zero inter-core synchronization during order matching.

### 2. **Lock-Free SPSC Ring Buffers**
Communication between the dispatcher and shard engines uses custom Single-Producer Single-Consumer queues built with `std::atomic` and explicit memory ordering (`acquire`/`release` semantics). No mutexes, no syscalls—just compare-and-swap operations.

**Why it matters:** In HFT, every nanosecond counts. Lock-free queues avoid kernel context switches and provide predictable, bounded latency even under heavy load.

### 3. **Cache Line Alignment & False Sharing Prevention**
The `Order` struct is aligned to 64 bytes (`alignas(64)`) to fit exactly into a CPU cache line. This prevents false sharing when multiple cores access different orders, keeping each core's L1 cache hot.

**Why it matters:** Modern CPUs can suffer 10-100x slowdowns when multiple cores invalidate each other's cache lines. Proper alignment ensures each core operates on independent cache lines.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Client Orders                          │
│                  (symbol_id, price, qty)                     │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
              ┌────────────────────────┐
              │   Dispatcher Thread    │
              │  (Hash-Based Routing)  │
              │  hash = symbol % cores │
              └────────────────────────┘
                           │
         ┌─────────────────┼─────────────────┐
         │                 │                 │
         ▼                 ▼                 ▼
   ┌─────────┐       ┌─────────┐       ┌─────────┐
   │ SPSC    │       │ SPSC    │       │ SPSC    │
   │ Queue 0 │       │ Queue 1 │       │ Queue N │
   └────┬────┘       └────┬────┘       └────┬────┘
        │                 │                 │
        ▼                 ▼                 ▼
   ┌─────────┐       ┌─────────┐       ┌─────────┐
   │ Shard 0 │       │ Shard 1 │       │ Shard N │
   │ Engine  │       │ Engine  │       │ Engine  │
   │ (Core 0)│       │ (Core 1)│       │ (Core N)│
   └────┬────┘       └────┬────┘       └────┬────┘
        │                 │                 │
        ▼                 ▼                 ▼
   OrderBook(A)      OrderBook(B)      OrderBook(C)
   OrderBook(D)      OrderBook(E)      OrderBook(F)
   [Price-Time]      [Price-Time]      [Price-Time]
```

**Flow:**
1. **Dispatcher** receives incoming orders and hashes symbol IDs to determine target shard
2. **SPSC Ring Buffer** transfers orders to the corresponding shard engine (lock-free)
3. **Shard Engine** (pinned to a dedicated CPU core) processes orders for its assigned symbols
4. **OrderBook** maintains bid/ask queues with strict Price-Time Priority matching

---

## Build & Run

### Prerequisites
- **Linux/macOS:** GCC 10+ or Clang 12+ (C++20 support)
- **Windows:** MSVC 2019+ or MinGW with C++20
- **CMake:** 3.15 or higher

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/DeepHFT.git
cd DeepHFT

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build . --config Release

# Run the simulator
./DeepHFT
```

### Windows (Visual Studio)
```cmd
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
Release\DeepHFT.exe
```

---

## Performance

**Current Benchmark (Preliminary):**
- **Throughput:** Processes ~1,000,000 orders in ~29ms on 12-core Intel i5-12500H
- **Latency:** Median order-to-match time <500ns (cache-hot scenarios)

*Benchmarks conducted on synthetic order flow with uniform symbol distribution. Real-world performance depends on workload characteristics and hardware.*

---

## Future Roadmap

### Planned Enhancements
- [ ] **Kernel Bypass Networking:** Integrate Solarflare/DPDK for sub-microsecond network I/O
- [ ] **Static Order Book Arrays:** Replace `std::map` with pre-allocated price-level arrays for O(1) insertions
- [ ] **NUMA-Aware Thread Pinning:** Bind threads to specific NUMA nodes to minimize cross-socket latency
- [ ] **FIX Protocol Support:** Add FIX 4.4 encoder/decoder for industry-standard message handling
- [ ] **Persistent Order Log:** Implement memory-mapped journaling for crash recovery

---

## Technical Details

### Dependencies
- **C++20 Standard Library:** `<atomic>`, `<thread>`, `<memory>`
- **POSIX Threads (pthreads):** For thread affinity (Linux/macOS)
- **CMake:** Build system generator

### Data Structures
- **Order Struct:** Cache-aligned (64 bytes) with atomics for lock-free access
- **Price-Time Queues:** Currently using `std::map<price, std::deque<Order>>` (planned migration to static arrays)
- **SPSC Ring Buffer:** Fixed-size circular buffer with power-of-2 capacity for fast modulo operations

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## Contact

**Author:** Swayam Prakash 
**Email:** swayam.prakash.ug23@nsut.ac.in

---

## Acknowledgments

Inspired by real-world HFT infrastructure at firms like Citadel, Jump Trading, and Virtu Financial. Built to demonstrate systems-level expertise in concurrent programming, memory models, and hardware-aware optimization.

---

**⚡ Built for speed. Engineered for scale.**