cmake_minimum_required(VERSION 3.30)
project(Tests)

find_package(GTest REQUIRED)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(Tests ${TEST_SOURCES})

target_link_libraries(Tests GTest::GTest GTest::Main)

target_include_directories(Tests PRIVATE ../Containers)
