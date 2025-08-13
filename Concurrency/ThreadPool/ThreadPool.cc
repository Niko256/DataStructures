#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads) : num_threads_(num_threads > 0 ? num_threads : std::thread::hardware_concurrency()) {}


/// [condition]: destructor must be called after close()
ThreadPool::~ThreadPool() {
    assert(stopped_ && "ThreadPool must be stopped before the destructioon");
}



/// Starts the worker threads
/// this method can be called only once
void ThreadPool::start() {
    // according to the condition start() and stop() are not called concurrenty 
    // and only one main thread is managing their execution => no mutexes needed
    assert(!started_); 
    started_.store(true);

    workers_.reserve(num_threads_);

    for (size_t i; i < num_threads_; ++i) {
        workers_.emplace_back([this]{
            worker_loop();
        });
    }
}


/// submits tasks for execution
/// [condition] : it must be called after start() and before stop()
void ThreadPool::submit(Task task) {
    // just ensure here that user follows the pool's lifecycle 'contract'
    assert(started_ && !stopped_);

   if (!task) {
        return;
   }

   tasks_.push(std::move(task));
}


/// Stops the pool [waits for all worker threads to finish]
/// => no new tasks will be submitted
/// ![must be called only once]!
void ThreadPool::stop() {
    // another ensuring that user follow stop rules 
    assert(started_ && !stopped_);
    
    stopped_.store(true);

    // closing the queue is th signal to worker threads to stop working for new tasks and exit their loop
    tasks_.close();

    // waits for all worker threads to complete their execution
    // [bc when stop() returns, all thread resources must be released]
    for (auto& w : workers_) {
        if (w.joinable()) {
            w.join();
        }
    }
}


/// Get a pointer to the ThreadPool instance for the current thread
ThreadPool* ThreadPool::current() {
    return current_pool_;
}


/// Main loop for each worker-thread
/// [every worker continuously pulls tasks from the queue and exec them, 'till queue is empty and closed]
void ThreadPool::worker_loop() {
    current_pool_ = this;

    while (true) {
        // pops blocks untill task is available OR the queue is closed
        std::optional<Task> task = tasks_.pop();

        if (!task) {
            // the worker's job is done
            break;
        }
    
        try {
            // executing the task
            (*task)();
        } catch(...) {

            // if a submitted task throws an exception that is doesn't handle internally, we catch it here
            //
            // !!! : A task that allows an exception to escape is violating its contract
            // !!! and has likely left application in a corrupted, unknowm state [broken invariants etc..]
            // !!! The ThreadPool's responsibility is to execute tasks, not to reason about their internal logic or error handling
            // !!! Thus continuing execution would be unsafe => the only responsible action is to terminate entire program to prevent further damage
 
            std::terminate();
        }
    }

}
