#include "Coro.hpp"
#include "sure/stack/mmap.hpp"
#include <cassert>

Coroutine::Coroutine(Body func) : f_(std::move(func)),
                                  stack_(Coroutine::allocate_stack()) {
    callee_context_.Setup(stack_.MutView(), this);
}

sure::stack::GuardedMmapExecutionStack Coroutine::allocate_stack(size_t size) {
    return sure::stack::GuardedMmapExecutionStack::AllocateAtLeastBytes(size);
}

void Coroutine::suspend() {
    callee_context_.SwitchTo(caller_context_);
}

void Coroutine::SuspendHandle::suspend() {
    self_->suspend();
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
    SuspendHandle suspend_context(this);

    /* SuspendContext is trivially copiable */
    f_(suspend_context);

    is_done_ = true;
    callee_context_.ExitTo(caller_context_);
}
