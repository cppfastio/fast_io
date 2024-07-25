include(${CMAKE_CURRENT_LIST_DIR}/../../get_cpm.cmake)

CPMAddPackage(
  NAME mimalloc
  GITHUB_REPOSITORY microsoft/mimalloc
  GIT_TAG v2.1.7
  # OPTIONS
)
