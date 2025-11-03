ADD_LIBRARY(Coroutine STATIC Coro.cc)

TARGET_LINK_LIBRARIES(Coroutine PUBLIC
  third_party_sure
  third_party_sure_stack
)


TARGET_INCLUDE_DIRECTORIES(Coroutine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/DataStructures/Concurrency/Coroutine>
)
