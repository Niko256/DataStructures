#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace data_structures::concurrency {

template <typename T>
class UnboundedBlockingQueue {
  private:
    std::deque<T> task_queue_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool is_closed_ = false;

  public:
    UnboundedBlockingQueue() = default;

    UnboundedBlockingQueue(const UnboundedBlockingQueue&) = delete;
    UnboundedBlockingQueue(UnboundedBlockingQueue&&) = delete;
    UnboundedBlockingQueue& operator=(const UnboundedBlockingQueue&) = delete;
    UnboundedBlockingQueue& operator=(UnboundedBlockingQueue&&) = delete;

    // Just basic push (add element to the end of the queue)
    // BUT! Behaviour is undefined if it called after close()
    void push(T item);


    // Waits for and retrives the element from the front of the queue
    // Returns std::nullopt immediately if the queue id closed or empty
    std::optional<T> pop();

    // Closes the queue for new additions
    // Wakes up all waiting consumers
    void close();
};

#include "Queue.tpp"
};
