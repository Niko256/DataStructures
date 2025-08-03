cmake_minimum_required(VERSION 3.30)

project(DataStructures LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/googletest)
enable_testing()

add_subdirectory(Sync)

add_subdirectory(tests)
