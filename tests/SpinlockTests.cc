#include "../src/Concurrency/Spinlock/Spinlock.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include <thread>
#include <vector>


using Spinlock = ds::concurrency::Spinlock;

class SpinlockBasicTest : public ::testing::Test {
  protected:
    Spinlock lock_;
};

TEST_F(SpinlockBasicTest, InitialStateUnlocked) {
    ASSERT_FALSE(lock_.is_locked());
}

TEST_F(SpinlockBasicTest, LockUnlockSequence) {
    EXPECT_TRUE(lock_.try_lock());
    EXPECT_TRUE(lock_.is_locked());

    lock_.unlock();
    EXPECT_FALSE(lock_.is_locked());
}

TEST_F(SpinlockBasicTest, DoubleLockFails) {
    lock_.lock();
    EXPECT_FALSE(lock_.try_lock());
    lock_.unlock();
}

class SpinlockConcurrencyTests : public ::testing::Test {

  protected:
    Spinlock lock_;
    std::atomic<int> counter_{0};

    const int num_threads = std::thread::hardware_concurrency();
    const int operations_per_thread = 10000;

    void SetUp() override {
        counter_.store(0);
    }

  public:
    void increment_with_lock() {
        for (int i = 0; i < operations_per_thread; ++i) {
            std::lock_guard<Spinlock> guard(lock_);
            ++counter_;
        }
    }

    void try_increment() {
        for (int i = 0; i < operations_per_thread;) {
            if (lock_.try_lock()) {
                ++counter_;
                lock_.unlock();
                ++i;
            }
        }
    }
};

TEST_F(SpinlockConcurrencyTests, NoDataRace) {
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(&SpinlockConcurrencyTests::increment_with_lock, this);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter_.load(), num_threads * operations_per_thread);
}

TEST_F(SpinlockConcurrencyTests, FairnessTest) {
    std::vector<int> acquisition_counts(num_threads, 0);
    std::vector<std::thread> threads;

    for (int id = 0; id < num_threads; id++) {
        threads.emplace_back([&, id] {
            for (int i = 0; i < operations_per_thread; ++i) {
                lock_.lock();
                ++acquisition_counts[id];
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                lock_.unlock();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int count : acquisition_counts) {
        EXPECT_GT(count, operations_per_thread * 0.5) << "Possible starvation detected";
    }
}

TEST_F(SpinlockConcurrencyTests, TryLockContention) {
    std::vector<std::thread> threads;
    std::atomic<int> successfully_tries{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&] {
            for (int j = 0; j < operations_per_thread;) {
                if (lock_.try_lock()) {
                    ++counter_;
                    lock_.unlock();
                    ++j;
                    ++successfully_tries;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }


    EXPECT_EQ(counter_.load(), num_threads * operations_per_thread);
    EXPECT_GE(successfully_tries.load(), num_threads * operations_per_thread);
}

class SpinlockTimeoutTest : public ::testing::Test {
  protected:
    Spinlock lock_;
};

// Verify try_lock_for succeeds when lock becomes available
TEST_F(SpinlockTimeoutTest, TryLockForSuccess) {
    // MAIN THREAD: Acquire lock to create contention
    lock_.lock();

    // BACKGROUND THREAD: Attempt to acquire lock with timeout
    auto future = std::async(std::launch::async, [&] {
        return lock_.try_lock_for(std::chrono::milliseconds(100));
    });

    // MAIN THREAD: Wait 50ms then release lock
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    lock_.unlock();

    // VERIFY: Background thread should have acquired lock
    EXPECT_TRUE(future.get());
}

TEST_F(SpinlockTimeoutTest, TryLockForTimeout) {
    // MAIN THREAD : Hold lock throughout test
    lock_.lock();

    // Start timing before try_lock_for
    auto start = std::chrono::steady_clock::now();

    // Try to acquire the lock (should fail)
    auto result = lock_.try_lock_for(std::chrono::milliseconds(50));
    auto end = std::chrono::steady_clock::now();

    // Verify timeout duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // VERIFY#1 : Should fail to acquire
    EXPECT_FALSE(result);

    // VERIFY#2 : Shouldn't wait much longer than timeout
    EXPECT_LE(duration.count(), 100) << "Waited too long";

    lock_.unlock();
}

// Verify try_lock_until succeeds when lock becomes available
TEST_F(SpinlockTimeoutTest, TryLockUntilSuccess) {
    lock_.lock();

    auto future = std::async(std::launch::async, [&] {
        // Set deadline 100ms in future
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
        return lock_.try_lock_until(deadline);
    });

    // Release lock at 50ms mark
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    lock_.unlock();

    // VERIFY: Should acquire before deadline
    EXPECT_TRUE(future.get());
}

// Test try_lock_until with past deadline
TEST_F(SpinlockTimeoutTest, TryLockUntilInPast) {
    auto past_time = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    EXPECT_FALSE(lock_.try_lock_until(past_time));
}

class SpinlockPerfTest : public ::testing::Test {
  protected:
    Spinlock lock_;
    const int heavy_work_iterations = 1000;

    // Simulate work in critical section
    void fake_work() {
        volatile int counter = 0;
        for (int i = 0; i < heavy_work_iterations; ++i) {
            counter += i * i;
        }
    }
};

// Measure basic lock/unlock overhead
TEST_F(SpinlockPerfTest, LowOverhead) {
    const int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::lock_guard<Spinlock> guard(lock_);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    double avg_lock_time = static_cast<double>(duration) / iterations;
    std::cout << "Average lock/unlock time: " << avg_lock_time << " ns\n";

    EXPECT_LT(avg_lock_time, 100);
}

// Test performance under heavy contention
TEST_F(SpinlockPerfTest, HighContention) {
    const int num_threads = std::thread::hardware_concurrency();
    const int operations_per_thread = 10000;
    std::atomic<int> counter{0};

    auto worker = [&] {
        for (int i = 0; i < operations_per_thread; ++i) {
            std::lock_guard<Spinlock> guard(lock_);
            fake_work();  // Simulate work while locked
            ++counter;
        }
    };

    std::vector<std::thread> threads;
    auto start = std::chrono::high_resolution_clock::now();

    // Create contention
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "High contention workload completed in " << duration << " ms\n";
    EXPECT_EQ(counter.load(), num_threads * operations_per_thread);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
