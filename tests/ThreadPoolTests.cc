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
TEST(ThreadPoolDeathTest, SubmitAfterStopAsserts) {
    auto routine = [] {
        ThreadPool pool(1);
        pool.start();
        pool.stop();
        pool.submit([] {});
    };

    ASSERT_DEATH(routine(), "started_ && !stopped_");
}
#endif
