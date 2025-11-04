Every stackful coroutine needs its own stack : a dedicated memory area where it keeps all its temporary data. This includes :

- local variables,

- function call history,

- return addresses,

- callee-saved processor registers.

---

The `ExecutionContext` is just a class that wraps the pointer to the coroutine's stack (`%rsp` register value).

When a coro suspends:

- It saves all callee-saved registers onto its stack

- It remembers the stack position in `ExecutionContext.rsp_`

When we resume a coro:

- It restores the stack position from `ExecutionContext.rsp_`

- It pops all the saved registers back from the stack

- Execution continues exactly where it left off

---

## The execution lifecycle

### 1. Creation

When you create a coro:

```C++

Coroutine coro([](SuspendHandle h)) {

	/* ... */

	h.suspend();

	/* ... */

});

```

=> In constructor:

- allocates a new stack via `sure-stack::AllocateAtLeastBytes(size)`
  - btw the size of the stack is 256kB by default ([emperically optimal](https://userver.tech/d7/dc8/md_en_2userver_2stack.html).)
- sets up `ExecutionContext` to use this stack

- prepares the stack with initial data

---

### 2. Setup

The Setup process prepares the coroutine's stack for its first execution. Hence we have to know two things :

- The Trampoline address : where to jump when execution starts

- The coroutine pointer : which coroutine to actually run (we know it bc we inherited our coroutine class from the ITrampoline abstract class)

---

### 3. The Trampoline

When we first call `coro.resume()`, something special happens:

- We switch to the coroutine's stack

- The processor jumps to a special 'trampoline' function in the `Sure` library

- The trampoline retrieves our coro pointer from the stack

- It calls `coro->Run()`

The trampoline acts as a universal adapter - `Sure` doesn't know about our specific Coroutine class, but it knows how to call `ITrampoline::Run()`

---

### 4. User Code execution

In `Run()`, it:

- Creates handle for ability to suspend our coroutine

- Calls the user's provided procedure with this handle

- The user's code runs until suspension point

---

### 5. Suspension

When the user call `suspend()`:

- Sure saves all calee-saved registers onto the coroutine's stack

- It saves the current stack position int the ExecutionContext.rsp\_

- It switches back to the caller's context

- Control returns to right after the `resume()` call

---

### 6. Subsequent resumptions

- It switches to the coro's stack

- It restores the saved stack position from `ExecutionContext.rsp_`

- It pops all the saved registers from the stack

- Execution continues from right after the last suspension point

No trampoline is involved here : we're just continuing existing execution.

---

### 7. Completion

- A final context switch back to the caller
