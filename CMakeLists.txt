cmake_minimum_required(VERSION 3.14)

project(DataStructures LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

# SMHasher
FetchContent_Declare(
    smhasher
    GIT_REPOSITORY https://github.com/aappleby/smhasher.git
    GIT_TAG master
)
FetchContent_MakeAvailable(smhasher)

enable_testing()

add_subdirectory(Concurrency)
add_subdirectory(SmartPtrs)

add_subdirectory(tests)
