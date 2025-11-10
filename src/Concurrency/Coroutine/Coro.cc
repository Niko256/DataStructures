#include "Coro.hpp"
#include "sure/stack/mmap.hpp"
#include <cassert>

namespace ds::runtime {

Coroutine::Coroutine(Routine func) : f_(std::move(func)),
                                     stack_(Coroutine::allocate_stack()) {
    callee_context_.Setup(stack_.MutView(), this);
}

sure::stack::GuardedMmapExecutionStack Coroutine::allocate_stack(size_t size) {
    return sure::stack::GuardedMmapExecutionStack::AllocateAtLeastBytes(size);
}

void Coroutine::suspend() {
    callee_context_.SwitchTo(caller_context_);
}

/* Resuming the execution of the coroutine
 * from preceding suspension point
 */
void Coroutine::resume() {
    assert(!is_done());
    caller_context_.SwitchTo(callee_context_);
}

bool Coroutine::is_done() const noexcept {
    return is_done_;
}

void Coroutine::Run() noexcept {
    f_();

    is_done_ = true;
    callee_context_.ExitTo(caller_context_);
}

};  // namespace ds::runtime
