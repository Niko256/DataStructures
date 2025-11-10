#include "../Coroutine/Coro.hpp"
#include "../ThreadPool/ThreadPool.hpp"
#include <functional>
#include <vvv/list.hpp>

namespace ds::concurrency::runtime {

// Fiber = Stackful coroutine x Scheduler

class Fiber : public vvv::IntrusiveListNode<Fiber> {
  private:
    using Scheduler = ds::concurrency::ThreadPool;
    using Procedure = std::function<void()>;

    ds::concurrency::Coroutine coro_;
    Scheduler& sched_;

  public:
    explicit Fiber(Scheduler&, Procedure);

    void schedule();

    void step();

    static void self();

    [[nodiscard]] Scheduler* get_current_scheduler();
};

};  // namespace ds::concurrency::runtime
