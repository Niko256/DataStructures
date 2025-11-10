#include "../Coroutine/Coro.hpp"
#include "../ThreadPool/ThreadPool.hpp"
#include <atomic>
#include <exception>
#include <functional>
#include <vvv/list.hpp>

namespace ds::concurrency::runtime {

// Fiber = Stackful coroutine x Scheduler

using Scheduler = ds::concurrency::ThreadPool;
using Procedure = std::function<void()>;

class Fiber : public vvv::IntrusiveListNode<Fiber> {
  private:
    ds::concurrency::Coroutine coro_;
    Scheduler& sched_;

    static thread_local Fiber* current_;

  public:
    explicit Fiber(Scheduler&, Procedure);

    void schedule();

    void step();

    static void set_current(Fiber*);

    static Fiber* current();

    Coroutine& get_coro() const;

    [[nodiscard]] Scheduler& current_scheduler() const;
};

};  // namespace ds::concurrency::runtime
