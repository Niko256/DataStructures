#pragma once

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <mutex>

class WaitGroup {
  private:
    std::mutex mtx_;
    std::condition_variable all_done_;
    size_t count_{0};

  public:
    void add(size_t count = 1) {
        std::unique_lock<std::mutex> lock(mtx_);
        count_ += count;
    }

    void done() {
        std::unique_lock<std::mutex> lock(mtx_);

        assert(count_ > 0);
        --count_;

        if (count_ == 0) {
            all_done_.notify_all();
        }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        all_done_.wait(lock, [this] {
            return count_ == 0;
        });
    }
};
