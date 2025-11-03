INCLUDE(FetchContent)

# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

# SMHasher
FetchContent_Declare(
    smhasher
    GIT_REPOSITORY https://github.com/aappleby/smhasher.git
    GIT_TAG master
)

FetchContent_Declare(
    sure
    GIT_REPOSITORY https://gitlab.com/Lipovsky/sure.git
    GIT_TAG master
)


FetchContent_Declare(
  sure-stack
  GIT_REPOSITORY https://gitlab.com/Lipovsky/sure-stack.git
  GIT_TAG master)

FetchContent_MakeAvailable(googletest smhasher sure sure-stack)


# ALIASES
if(sure_POPULATED)
  add_library(third_party_sure INTERFACE)
  target_include_directories(third_party_sure INTERFACE
      ${sure_SOURCE_DIR}/source)
endif()

if(sure-stack_POPULATED)
  add_library(third_party_sure_stack INTERFACE)
  target_include_directories(third_party_sure_stack INTERFACE
      ${sure-stack_SOURCE_DIR}/source)
endif()

if(smhasher_POPULATED)
  add_library(third_party_smhasher INTERFACE)
  target_include_directories(third_party_smhasher INTERFACE
      ${smhasher_SOURCE_DIR}/src)
endif()
