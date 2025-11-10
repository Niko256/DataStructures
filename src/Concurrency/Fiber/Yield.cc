#include "Yield.hpp"

namespace ds::concurrency::fiber {

void yield() {
    auto f = runtime::Fiber::current();
    f->get_coro().suspend();
}

};  // namespace ds::concurrency::fiber
