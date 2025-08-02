cmake_minimum_required(VERSION 3.30)

project(Data_Structures LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Sync)

add_subdirectory(tests)
