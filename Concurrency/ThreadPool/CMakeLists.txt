add_library(ThreadPool STATIC ThreadPool.cc)

target_include_directories(
    ThreadPool
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/DataStructures/Concurrency/ThreadPool>
)
