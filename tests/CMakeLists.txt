cmake_minimum_required(VERSION 3.30)
project(Tests LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(HASH_SOURCES ${smhasher_SOURCE_DIR}/src/City.cpp
                 ${smhasher_SOURCE_DIR}/src/MurmurHash3.cpp)

file(GLOB TEST_SOURCES "*.cc")

add_executable(Tests ${TEST_SOURCES} ${HASH_SOURCES})

target_link_libraries(Tests gtest gtest_main Spinlock SmartPtrs ThreadPool)

target_include_directories(
  Tests
  PRIVATE ${CMAKE_SOURCE_DIR}/Containers
          ${smhasher_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/Concurrency/Spinlock
          ${CMAKE_SOURCE_DIR}/Concurrency/ThreadPool)

add_test(NAME DS_TESTS COMMAND $<TARGET_FILE:Tests>)
