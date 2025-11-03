ADD_LIBRARY(Spinlock STATIC Spinlock.cc)

TARGET_INCLUDE_DIRECTORIES(
  Spinlock
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/DataStructures/Concurrency/Spinlock>
)
