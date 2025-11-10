#pragma once

#include "../Coroutine/Routine.hpp"
#include "Fiber.hpp"

namespace ds::fiber {

void go(runtime::Scheduler&, runtime::Routine);

void go(runtime::Routine);

};  // namespace ds::fiber
