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
    GIT_TAG master
)

FetchContent_Declare(
    vvv
    GIT_REPOSITORY https://gitlab.com/Lipovsky/vvv.git
    GIT_TAG master
)


FetchContent_MakeAvailable(googletest smhasher sure sure-stack vvv)


# ALIASES
IF(sure_POPULATED)
  ADD_LIBRARY(third_party_sure ALIAS sure)
ENDIF()

IF(sure-stack_POPULATED)
  ADD_LIBRARY(third_party_sure_stack INTERFACE)
  TARGET_INCLUDE_DIRECTORIES(third_party_sure_stack INTERFACE
      ${sure-stack_SOURCE_DIR}/source)
ENDIF()

IF (vvv_POPULATED)
  MESSAGE(STATUS "vvv source dir: ${vvv_SOURCE_DIR}")

  EXECUTE_PROCESS(COMMAND find ${vvv_SOURCE_DIR} -name "*.hpp" -o -name "*.h"
                  OUTPUT_VARIABLE VVV_HEADERS)
  MESSAGE(STATUS "Found vvv headers: ${VVV_HEADERS}")

  ADD_LIBRARY(third_party_vvv INTERFACE)
  TARGET_INCLUDE_DIRECTORIES(third_party_vvv INTERFACE
    ${vvv_SOURCE_DIR}/source)
ENDIF()

IF(smhasher_POPULATED)
  ADD_LIBRARY(third_party_smhasher INTERFACE)
  TARGET_INCLUDE_DIRECTORIES(third_party_smhasher INTERFACE
      ${smhasher_SOURCE_DIR}/src)
ENDIF()
