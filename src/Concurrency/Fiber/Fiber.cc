#include "Fiber.hpp"
#include <exception>

namespace ds::concurrency::runtime {

thread_local Fiber* Fiber::current_ = nullptr;

Fiber::Fiber(Scheduler& sched, Procedure routine)
    : sched_(sched), coro_(std::move(routine)) {}

void Fiber::schedule() {
    sched_.submit([this] {
        this->step();
    });
}

void Fiber::step() {
    auto prev_fiber = current_;
    current_ = this;

    coro_.resume();

    current_ = prev_fiber;

    /* polling the completion */
    if (!get_coro().is_done()) {
        /* re-subscription (or rescheduling) */
        this->schedule();
    } else {
        delete this;
    }
}

/* get current fiber */
Fiber* Fiber::current() {
    return current_;
}

void Fiber::set_current(Fiber* new_current) {
    current_ = new_current;
}

/* get internal coroutine */
Coroutine& Fiber::get_coro() const {
    return coro_;
}

/* get internal scheduler */
Scheduler& Fiber::current_scheduler() const {
    return sched_;
}
};  // namespace ds::concurrency::runtime
