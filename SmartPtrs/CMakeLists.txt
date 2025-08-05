add_library(SmartPtrs INTERFACE)

target_include_directories(
    SmartPtrs INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                   $<INSTALL_INTERFACE:include/DataStructures/SmartPtrs>)
