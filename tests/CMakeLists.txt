cmake_minimum_required(VERSION 3.30)
project(Tests)

find_package(GTest REQUIRED)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(HASH_SOURCES
    ${CMAKE_SOURCE_DIR}/external/smhasher/src/City.cpp
    ${CMAKE_SOURCE_DIR}/external/smhasher/src/MurmurHash3.cpp
)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(Tests 
    ${TEST_SOURCES}
    ${HASH_SOURCES}
)

target_link_libraries(Tests GTest::GTest GTest::Main)

target_include_directories(Tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/Containers
    ${CMAKE_SOURCE_DIR}/external/smhasher/src
)
