#pragma once

#include "../Coroutine/Coro.hpp"
#include "../Coroutine/Routine.hpp"
#include "../ThreadPool/ThreadPool.hpp"
#include <vvv/list.hpp>

namespace ds::runtime {

// Fiber = Stackful coroutine x Scheduler

using Scheduler = ds::runtime::ThreadPool;

class Fiber : public vvv::IntrusiveListNode<Fiber> {
  private:
    ds::runtime::Coroutine coro_;
    Scheduler& sched_;

    static thread_local Fiber* current_;

  public:
    explicit Fiber(Scheduler&, Routine);

    void schedule();

    void step();

    static void set_current(Fiber*);

    static Fiber* current();

    Coroutine& get_coro();

    [[nodiscard]] Scheduler& current_scheduler() const;
};

};  // namespace ds::runtime
