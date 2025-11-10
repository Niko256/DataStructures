#include "Preamble.hpp"

template <typename Rep, typename Period>
bool Spinlock::try_lock_for(const ::std::chrono::duration<Rep, Period>& rel_time) noexcept {
    auto start_time = ::std::chrono::steady_clock::now();

    // Initial attempt to acquire the lock
    if (try_lock()) {
        return true;
    }

    // If not acquired => enter a spinning loop with backoff, but with timeout
    uint32_t backoff = SPIN_INITIAL_BACKOFF;

    for (;;) {
        if (::std::chrono::steady_clock::now() - start_time >= rel_time) {
            // Timeout, lock is not acquired
            return false;
        }

        // Therefore we enter the inner loop where we won't acquire the lock
        // and prefer checking with relaexd loads to reduce contention
        for (uint32_t i = 0; i < backoff; ++i) {
            CPU_PAUSE();

            if (::std::chrono::steady_clock::now() - start_time >= rel_time) {
                return false;
            }

            // If the lock becomes free => break early to attempt of acquiring the lock  in outer loop
            if (!flag_.load()) {
                break;
            }
        }

        // Attempt to acquire the lock after backoff
        // This check is outside the inner backoff loop to limit frequency of exchanges
        if (!flag_.load()) {
            if (!flag_.exchange(true)) {
                return true;
            }
        }

        // Increase the backoff for the next iteration
        backoff = ::std::min(backoff << 1, SPIN_MAX_BACKOFF);
    }
}

template <typename Clock, typename Duration>
bool Spinlock::try_lock_until(const ::std::chrono::time_point<Clock, Duration>& abs_time) noexcept {

    // First check if deadline already passed
    if (Clock::now() >= abs_time) {
        return false;
    }

    if (try_lock()) {
        return true;
    }

    // If not acquired => enter a spinning loop with backoff, but with absolute deadline
    uint32_t backoff = SPIN_INITIAL_BACKOFF;

    for (;;) {
        // Check if the current time has passed the absolute deadline
        if (Clock::now() >= abs_time) {
            return false;
        }

        // Active spinning with backoff (simular to 'lock()'s' inner loop)
        for (int i = 0; i < backoff; ++i) {
            CPU_PAUSE();

            // Check if deadline passed during the pause
            if (Clock::now() >= abs_time) {
                return false;
            }

            // If the lock becomes free => break
            if (!flag_.load()) {
                break;
            }
        }

        if (!flag_.load()) {
            if (!flag_.exchange(true)) {  // Set
                return true;
            }
        }

        backoff = ::std::min(backoff << 1, SPIN_MAX_BACKOFF);
    }
}
