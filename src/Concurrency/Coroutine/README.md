## My Stackful Coroutine implementation.

A Coroutine is a function that can pause its execution and yield control back to the caller, then later resume from where itleft off. Unlike regular functions, coroutines maintain their state between suspensions

![](https://i.imgur.com/RI9ejZP.png)

---

Every stackful coroutine needs its own stack : a dedicated memory area where it keeps all its temporary data.

This includes :

- local variables,

- function call history,

- return addresses,

- callee-saved processor registers.

---

The `ExecutionContext` is just a class that wrap the pointer to the coroutine's stack (`%rsp` register value).

When a coro suspends:

- We save all callee-saved registers onto its stack

- We remember the stack position in `ExecutionContext.rsp_`

When we resume a coro:

- We restore the stack position from `ExecutionContext.rsp_`

- We pop all the saved registers back from the stack

- Execution continues exactly where it left off

---

## The execution lifecycle

1. Creation

When you create a coro:

```C++

Coroutine coro([](SuspendHandle h)) {

	/* ... */

	h.suspend();

	/* ... */

});

```

=> In constructor a new stack allocates, `ExecutionContext` sets up to use this stack and prepares the stack with initial data

2. Setup

The Setup process prepares the coroutine's stack for its first execution. Hence we have to know to things :

- The Trampoline adress : where to jump when execution starts

- The coroutine pointer : which coroutine to actually run (we know it bc we inherited our coroutine class from the ITrampoline abstract class)

3. The Trampoline

When we first call `coro.resume()`, something special happens:

- We switch to the coroutine's stack

- The processor jumps to a special 'trampoline' function in the `Sure` library

- The trampoline retrieves our coro pointer from the stack

- It calls `coro->Run()`

The trampoline acts as a universal adapter - `Sure` doesn't know about our specific Coroutine class, but it knows how to call `ITrampoline::Run()`

4. User Code execution

In `Run()`, we:

- Create handle for ability to suspend our coroutine

- Call the user's provided procedure with this handle

- The user's code runs untill suspension pointer

5. Suspension

When the user call `suspend()`:

- Sure save all calee-saved registers onto the coroutine's stack

- We save the current stack position int the ExecutionContext.rsp\_

- We switch back to the caller's context

- Control returns to right after the `resume()` call

6. Subsequent resumptions

- We switch to the coro's stack

- We store the saved stack position from `ExecutionContext.rsp_`

- We pop all the saved registers from the stack

- Execution continues from right after the last suspension point

No trampoline is involved here : we're just continuing existing execution.

7. Completion

- A final context switch back to the caller

---

## Dependencies

This implementation builds upon two core libraries:

- [Sure library](https://gitlab.com/Lipovsky/sure.git) : provides all low-level context switching mechanisms

- [Sure-stack library](https://gitlab.com/Lipovsky/sure-stack.git) : implements `GuardedMmapExecutionStack` as a stack for coroutines

For deeper understanding of the internal mechanics (including trampoline magic :) ), explore the source code of these libraries!
