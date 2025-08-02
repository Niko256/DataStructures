add_library(Spinlock STATIC Spinlock.cc)

target_include_directories(
  Spinlock PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                  $<INSTALL_INTERFACE:include/Data_Structures/Sync/Spinlock>)
