#include "Go.hpp"

namespace ds::fiber {

void go(ds::runtime::Scheduler& sched, runtime::Routine proc) {
    auto newbie = new ds::runtime::Fiber(sched, std::move(proc));
    newbie->schedule();
}

void go(runtime::Routine proc) {
    go(ds::runtime::Fiber::current()->current_scheduler(), std::move(proc));
}

};  // namespace ds::fiber
