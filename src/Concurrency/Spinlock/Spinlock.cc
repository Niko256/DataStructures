#include "Spinlock.hpp"
#include "Preamble.hpp"
#include <algorithm>
#include <atomic>

namespace ds::concurrency {

void Spinlock::lock() noexcept {
    // Initial value for exp.backoff (exponintial backoff reduces contention by increasing delay for each retry)
    uint32_t backoff = SPIN_INITIAL_BACKOFF;

    // A: 'TTAS' attempt (Test-and-Test-and-Set)
    for (;;) {
        // Test load: Check if the lock is potentially free
        if (!flag_.load()) {
            // Set: try to acquire the lock
            //
            if (!flag_.exchange(true)) {
                // Successfully acquired the lock
                return;
            }
        }

        // B: Active spinning with backoff (spin loop)
        // The lock is currently held (or was just taken by another thread)
        // Spin and wait for it become free with exponintial backoff
        //
        // We __don't__ attempt to acquire the lock in this inner loop
        // bc we want to minimize bus contention and cache line bouncing
        for (uint32_t i = 0; i < backoff; ++i) {
            CPU_PAUSE();
            // relaxed load to check if the lock becomes free => break early from this backoff period
            if (!flag_.load()) {
                // lock appears free, exit this inner loop and try to acquire in outer loop
                break;
            }
        }

        // Double the backoff period for the next iteration, up to a maximum limit
        // this helps to progressively reduce the frequency of `exchange` attempts under high contention,
        // further minimizing bus traffic
        backoff = ::std::min(backoff << 1, SPIN_MAX_BACKOFF);
    }
}

// We trying to acquire the lock without blocks, returning immediately if busy
bool Spinlock::try_lock() noexcept {
    // This is simular to the first 'test' in TTAS
    if (flag_.load()) {
        // lock is currently held, cannot acquire
        return false;
    }

    return !flag_.exchange(true);
}

bool Spinlock::is_locked() const noexcept {
    return flag_.load();
}

void Spinlock::unlock() noexcept {
    // releasing the lock
    flag_.store(false);
}
}  // namespace ds::concurrency
