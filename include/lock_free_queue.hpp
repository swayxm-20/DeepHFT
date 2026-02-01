#pragma once
#include <vector>
#include <atomic>
#include <optional>

template <typename T>
class LockFreeQueue {
public:
    explicit LockFreeQueue(size_t size) : buffer_(size), size_(size), head_(0), tail_(0) {}

    // Only called by Producer (Main Thread)
    bool push(const T& item) {
        const auto current_tail = tail_.load(std::memory_order_relaxed);
        const auto next_tail = (current_tail + 1) % size_;

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // Queue Full
        }

        buffer_[current_tail] = item;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    // Only called by Consumer (Worker Thread)
    std::optional<T> pop() {
        const auto current_head = head_.load(std::memory_order_relaxed);

        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt; // Queue Empty
        }

        T item = buffer_[current_head];
        head_.store((current_head + 1) % size_, std::memory_order_release);
        return item;
    }

private:
    std::vector<T> buffer_;
    const size_t size_;
    alignas(64) std::atomic<size_t> head_; // Padding to separate cache lines
    alignas(64) std::atomic<size_t> tail_;
};