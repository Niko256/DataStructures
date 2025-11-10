#pragma once

#include <functional>
#include <sure/context.hpp>
#include <sure/stack/mmap.hpp>
#include <sure/trampoline.hpp>

namespace ds::concurrency {

/*
 * Stackful coroutine
 *
 * A cooperative execution unit with it's own stack that can suspend
 * and resume execution while preserving its state on the stack.
 *
 * ITrampoline is a abstract class that represents
 * an executable entity within the context switching mechanism.
 *
 * It serves as a universal entry point that
 * allows the Sure library to activate execution on a new stack
 * without knowing concrete type of the executable entity.
 *
 */
class Coroutine : private sure::ITrampoline {
  private:
    sure::ExecutionContext callee_context_; /* The coroutine's execution context */
    sure::ExecutionContext caller_context_; /* Caller's execution context */
    sure::stack::GuardedMmapExecutionStack stack_;
    bool is_done_ = false;

    /*
     * ! SuspendHandle provides controlled access to the suspension operation on the coro.
     * Provides safe suspension capability while protecting it from the coroutine's
     * internal statefrom direct manipulation
     */
    struct SuspendHandle {
      private:
        friend class Coroutine;

        Coroutine* self_;

      public:
        explicit SuspendHandle(Coroutine* coro) : self_(coro) {}

        void suspend();
    };

    /* ! User-provided procedure that receives suspension capability */
    using Body = std::function<void(SuspendHandle)>;

    Body f_;

  public:
    /* Constructs coro with the given procedure */
    explicit Coroutine(Body);

    void suspend();

    /* Transfer execution to the coro.
     * If it is the first call : begins execution from the start
     * Subsequent calls : resumes from the suspension point
     */
    void resume();

    /* Returns true if the coro has completed execution */
    bool is_done() const noexcept;

  private:
    /* Run() is the activation point called by Sure when execution begins
     *
     * [!] This method :
     *    \ create SuspendHandle for user code
     *    \ invokes the user's procedure
     *    \ marks completion and performs final context switch via ExitTo() method
     *
     * [!] Called exactly once per coroutine lifetime through ITrampoline interface
     */
    void Run() noexcept override;

    /* Allocates guarded stack for coroutine execution */
    static sure::stack::GuardedMmapExecutionStack allocate_stack(size_t size = 256 * 1024);
};  // namespace sure::ITrampoline

};  // namespace ds::concurrency
