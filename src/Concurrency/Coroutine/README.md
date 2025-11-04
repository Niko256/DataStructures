## My Stackful Coroutine implementation.

A Coroutine is a function that can pause its execution and yield control back to the caller, then later resume from where it left off. Unlike regular functions, coroutines maintain their state between suspensions

![](https://i.imgur.com/RI9ejZP.png)

---

## Dependencies

This implementation builds upon two core libraries:

- [Sure library](https://gitlab.com/Lipovsky/sure.git) : provides all low-level context switching mechanisms

- [Sure-stack library](https://gitlab.com/Lipovsky/sure-stack.git) : implements `GuardedMmapExecutionStack` as a stack for coroutines (and other executable entities)

For deeper understanding of the internal mechanics (including trampoline magic :) ), explore these libraries [ﾉ^_^]ﾉ
