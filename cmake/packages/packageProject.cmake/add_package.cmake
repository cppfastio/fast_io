file(GLOB_RECURSE PackageProject_patches CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_LIST_DIR}/patches/*.patch"
)

CPMAddPackage(
  NAME PackageProject.cmake
  VERSION 1.11.2
  GIT_REPOSITORY https://github.com/TheLartians/PackageProject.cmake.git PATCHES
                                                                         ${PackageProject_patches}
)
