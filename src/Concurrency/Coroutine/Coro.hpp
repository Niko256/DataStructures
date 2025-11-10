#pragma once

#include "Routine.hpp"
#include <sure/context.hpp>
#include <sure/stack/mmap.hpp>
#include <sure/trampoline.hpp>

namespace ds::runtime {

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
  public:
    /* Constructs coro with the given procedure */
    explicit Coroutine(Routine);

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


  private:
    sure::ExecutionContext callee_context_; /* The coroutine's execution context */
    sure::ExecutionContext caller_context_; /* Caller's execution context */
    sure::stack::GuardedMmapExecutionStack stack_;
    bool is_done_ = false;


    Routine f_;
};  // namespace sure::ITrampoline

};  // namespace ds::runtime
