ADD_LIBRARY(Fiber STATIC Fiber.cc Yield.cc Go.cc)


TARGET_LINK_LIBRARIES(Fiber PUBLIC
  third_party_vvv
  Coroutine
)


TARGET_INCLUDE_DIRECTORIES(Fiber PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/DataStructures/Concurrency/Fiber>
)
