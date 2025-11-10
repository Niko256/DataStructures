ADD_SUBDIRECTORY(Spinlock)
ADD_SUBDIRECTORY(ThreadPool)
# ADD_SUBDIRECTORY(Mutex)
ADD_SUBDIRECTORY(Fiber)
ADD_SUBDIRECTORY(Coroutine)

ADD_LIBRARY(Concurrency INTERFACE
  Spinlock
  ThreadPool
  Coroutine
  Fiber
)
