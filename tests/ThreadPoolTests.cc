#include <atomic>
#include <gtest/gtest.h>
#include "../Concurrency/WaitGroup/WaitGroup.hpp"
#include "../Concurrency/ThreadPool/Queue.hpp"
#include "../Concurrency/ThreadPool/ThreadPool.hpp"
#include <future>
#include <memory>
#include <mutex>
#include <numeric>
#include <set>
#include <thread>


using namespace std::chrono_literals;

using WaitGroup = data_structures::concurrency::WaitGroup; 
using ThreadPool = data_structures::concurrency::ThreadPool;


// =============================================
// ============ WAITGROUP TESTS ============
// =============================================
//

TEST(WaitGroupTest, ConcurrentDone) {
    WaitGroup wg;
    const size_t task_count = 1000;
    std::vector<std::thread> threads;
    std::atomic<int> cnt;

    wg.add(task_count);

    for (size_t i = 0; i < task_count; ++i) {
        threads.emplace_back([&]{
            cnt.fetch_add(1);
            wg.done();
    });}

    wg.wait();

    for (auto& t : threads) {
        t.join();
    }

    SUCCEED();
}



TEST(WaitGroupTest, BlocksUntilDone) {
    WaitGroup wg;
    std::atomic<bool> task_done = false;
    
    wg.add(1);

    std::jthread task_thread([&] {
        std::this_thread::sleep_for(100ms);
        task_done.store(true);

        wg.done();
    });

    wg.wait();

    ASSERT_TRUE(task_done);
}



// =============================================
// ============ TREADPOOL TESTS ============
// =============================================
//
class ThreadPoolTests : public ::testing::Test {
  protected:
    
    std::unique_ptr<ThreadPool> pool_;

    void SetUp() override {
        pool_ = std::make_unique<ThreadPool>(4);
        pool_->start();
    }

    void TearDown() override {
        pool_->stop();
    }
};


TEST_F(ThreadPoolTests, ExecutesOneTask) {
    std::promise<void> pr;
    auto future = pr.get_future();

    pool_->submit([&]{
        pr.set_value();
    });

    auto status = future.wait_for(1s);
    ASSERT_EQ(status, std::future_status::ready);
}


TEST_F(ThreadPoolTests, ExecuteManyTasks) {
    const size_t task_count = 10000;
    WaitGroup wg;
    std::atomic<size_t> tasks_executed{0};

    wg.add(task_count);
    for (size_t i = 0; i < task_count; ++i) {
        pool_->submit([&] {
            tasks_executed.fetch_add(1);
            wg.done();
        });    
    }

    wg.wait();

    ASSERT_EQ(tasks_executed.load(), task_count);
}



TEST_F(ThreadPoolTests, TasksRunOnDifferentThreads) {
    const size_t task_count = 50;
    WaitGroup wg;
    std::mutex mtx;
    std::set<std::thread::id> threads_ids;

    wg.add(task_count);
    for (size_t i = 0; i < task_count; ++i) {
        pool_->submit([&]{
            {
                std::lock_guard<std::mutex> lock(mtx);
                threads_ids.insert(std::this_thread::get_id());
            }

            wg.done();
        });
    }

    // ensure that all tasks has been hinished
    wg.wait();

    ASSERT_GT(threads_ids.size(), 1);
    ASSERT_LE(threads_ids.size(), 4);
}



TEST_F(ThreadPoolTests, CurrentMethod) {
    ASSERT_EQ(ThreadPool::current(), nullptr);

    std::promise<ThreadPool*> p;
    auto f = p.get_future();

    pool_->submit([&p, this] {
        p.set_value(ThreadPool::current());
    });
    
    ThreadPool* current_pool_ptr = f.get(); 
    
    ASSERT_EQ(current_pool_ptr, pool_.get());
}



TEST_F(ThreadPoolTests, ZeroThreadsPool) {
    ThreadPool pool(0); // !std::hardware_concurrency here!
    pool.start();

    std::promise<void> p;
    auto f = p.get_future();

    pool.submit([&]{
        p.set_value();
    });

    auto status = f.wait_for(1s);
    ASSERT_EQ(status, std::future_status::ready);

    pool.stop();
}


#if NDEBUG
#warning "Death tests are disabled in release builds (NDEBUG is defined)."
#else
/// Death test. We want to verify that submit() triggers an assert 
/// if it's called after stop() has been initiated but before it has completed
TEST(ThreadPoolDeathTest, SubmitAfterStopAsserts) {
    auto routine = [] {
        ThreadPool pool(1);
        pool.start();

        WaitGroup wg;

        // so, we want to simulate race condition here
        std::atomic<bool> stop_called = false;

        // the goal is to call submit() precisely when stop() has started but IS BLOCKED, 
        // waiting for a worker thread to finish
        wg.add(1);

        /// We submit a task that will never finish. This will cause .stop()
        // to get stuck in its .join() loop, allowing us to test
        // the state of the pool "mid-shutdown".
        pool.submit([&]{
            while (!stop_called.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            // By NOT calling a WaitGroup's Done(), this task effectively hangs forever
        });

        // Give the worker thread a moment to pick up the task from the queue
        // After this sleep, we can be reasonably sure the task is running
        std::this_thread::sleep_for(20ms);

        std::thread stop_thread([&]{
            pool.stop(); // This call will hang on worker.join()
        });

        stop_called.store(true, std::memory_order_release);

        // After this sleep, we are confident that the stopped_ flag inside
        // the pool has been set to 'true', and the task queue has been closed
        std::this_thread::sleep_for(20ms);

        // This is the core of the test. We are now in the perfect state:
        //  .stop() has been called (stopped_ is true)
        //  .stop() has NOT completed (the worker thread is still "alive").
        // Calling submit() now should trigger the assert(started_ && !stopped_)
        pool.submit([]{});

        stop_thread.join();
    };

    ASSERT_DEATH(routine(), "started_ && !stopped_");
}
#endif
