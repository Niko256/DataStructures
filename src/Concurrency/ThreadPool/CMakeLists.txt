ADD_LIBRARY(ThreadPool STATIC ThreadPool.cc)

TARGET_INCLUDE_DIRECTORIES(
    ThreadPool
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/DataStructures/Concurrency/ThreadPool>
)
