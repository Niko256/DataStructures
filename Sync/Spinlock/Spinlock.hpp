#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>

#if defined(__i386__) || defined(__x86_64__)
#    include <immintrin.h>
#    define CPU_PAUSE() _mm_pause()
#elif defined(__aarch64__) || defined(__arm__)
#    define CPU_PAUSE() asm volatile("yield")
#else
#    define CPU_PAUSE() static_cast<void>(0)  // No-op
#    warning "Unknown architecture, CPU_PAUSE is a no-op"
#endif

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
