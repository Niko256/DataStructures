#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>

namespace data_structures::concurrency {

class Spinlock {
  private:
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr uint32_t SPIN_INITIAL_BACKOFF = 4;
    static constexpr uint32_t SPIN_MAX_BACKOFF = 1024;

    alignas(CACHE_LINE_SIZE) std::atomic<bool> flag_{false};

  public:
    Spinlock() = default;
    ~Spinlock() = default;

    Spinlock(Spinlock&&) = delete;
    Spinlock& operator=(Spinlock&&) = delete;
    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    void lock() noexcept;
    bool try_lock() noexcept;
    bool is_locked() const noexcept;
    void unlock() noexcept;

    template <typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time) noexcept;

    template <typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time) noexcept;
};

#include "Spinlock.tpp"
}// namespace data_structures::concurrency
