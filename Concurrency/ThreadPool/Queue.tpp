#include <mutex>

template <typename T>
void UnboundedBlockingQueue<T>::push(T item) {
    {
        std::unique_lock<std::mutex> lock(mtx_);

        task_queue_.push_back(std::move(item));
    }

    // Notify one waiting thread that new element is available
    cv_.notify_one();
}

template <typename T>
std::optional<T> pop() {
    std::unique_lock<std::mutex> lock(mtx_);

    // Waits untill queue is not empty OR it has been closed and no more items will be ever produced
    cv_.wait(lock, [this] {
        return !task_queue_.is_empty() || is_closed_;
    });

    // After waking up we have to check the conditions of the queue
    if (task_queue_.is_empty() && is_closed_) {
        return std::nullopt;
    }

    // Otherwise, there must be an item in the queue
    T item = std::move(task_queue_.front());
    task_queue_.pop_front();

    return item;
}



template <typename T>
void UnboundedBlockingQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(mtx_);

        closed_ = true;
    }
    // We must notify all threads that our queue has closed
    // so any thread blocked in the pop() method
    // can wake up, see if the queue is closed and empty, and exit
    cv_.notify_all();
}
