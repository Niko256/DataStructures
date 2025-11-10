#include "../src/Concurrency/ThreadPool/ThreadPool.hpp"
#include "../src/Concurrency/WaitGroup/WaitGroup.hpp"

#include "../src/Concurrency/Fiber/Fiber.hpp"
#include "Go.hpp"
#include <fmt/printf.h>
#include <gtest/gtest.h>
#include <memory>

using namespace ds::runtime;

using Scheduler = ds::runtime::ThreadPool;
using WaitGroup = ds::sync::WaitGroup;

class FiberTest : public ::testing::Test {
  protected:
    std::unique_ptr<Scheduler> sched_;
    std::unique_ptr<Fiber> fiber_;

    void SetUp() override {
        sched_ = std::make_unique<Scheduler>(4);
        sched_->start();
    }

    void TearDown() override {
        sched_->stop();
    }
};

TEST_F(FiberTest, SimpleExecution) {
    WaitGroup wg;
    wg.add(1);

    ds::fiber::go(*sched_, [&wg] {
        // fmt::print("Here we go...");
        wg.done();
    });

    wg.wait();
}

TEST_F(FiberTest, AfewSteps) {
}

TEST_F(FiberTest, ExceptionInFiber) {
}

TEST_F(FiberTest, TheHordeOfFibers) {
}

TEST_F(FiberTest, Rescheduling1) {
}

TEST_F(FiberTest, Rescheduling2) {
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
