cmake_minimum_required(VERSION 3.18)

project(
  fast_io
  LANGUAGES CXX
  VERSION 2.0
)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE
  )
  # Set the possible values of build type for cmake-gui
  set_property(
    CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
  )
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set(${PROJECT_NAME}_DEFAULT_VAL ON)
else()
  set(${PROJECT_NAME}_DEFAULT_VAL OFF)
endif()

option(${PROJECT_NAME}_INSTALL_MANUALS "Install manuals" ${${PROJECT_NAME}_DEFAULT_VAL})
option(${PROJECT_NAME}_BUILD_I18N_DLLS "Build i18n runtime dlls" ON)
set(additional_parts TESTS FUZZERS EXAMPLES BENCHMARKS)
foreach(PART ${additional_parts})
  string(TOLOWER ${PART} part_lowercase)
  option(${PROJECT_NAME}_ENABLE_${PART} "Enable ${part_lowercase}" ${${PROJECT_NAME}_DEFAULT_VAL})
  if(${${PROJECT_NAME}_ENABLE_${PART}})
    option(${PROJECT_NAME}_ENABLE_${PART}_ADD_ASAN "Build ${part_lowercase} with address sanitizer"
           ON
    )
    option(${PROJECT_NAME}_ENABLE_${PART}_ADD_MSAN "Build ${part_lowercase} with memory sanitizer"
           OFF
    )
    option(${PROJECT_NAME}_ENABLE_${PART}_ADD_UBSAN
           "Build ${part_lowercase} with undefined behaviour sanitizer" OFF
    )
  endif()
endforeach()

file(GLOB_RECURSE ${PROJECT_NAME}_headers CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/**.h"
)
message(DEBUG "${PROJECT_NAME}_headers: ${${PROJECT_NAME}_headers}")

add_library(${PROJECT_NAME} INTERFACE)

# if it was not header-only ... # being a cross-platform target, we enforce standards conformance on
# MSVC target_compile_options(${PROJECT_NAME} PUBLIC
# "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

target_include_directories(
  ${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
   OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin"
   OR CMAKE_SYSTEM_NAME STREQUAL "Msys"
)
  target_link_libraries(${PROJECT_NAME} PUBLIC ntdll)
endif()

if(${${PROJECT_NAME}_BUILD_I18N_DLLS})
  add_subdirectory(src)
endif()

if(${${PROJECT_NAME}_BUILD_FUZZERS})
  add_subdirectory(fuzzing)
endif()

if(${${PROJECT_NAME}_BUILD_BENCHMARKS})
  add_subdirectory(benchmark)
endif()

if(${${PROJECT_NAME}_ENABLE_TESTS})
  include(CTest)
  add_subdirectory(tests)
endif()

if(${${PROJECT_NAME}_BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()

# ======================== INSTALL ==========================

string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpm.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/packages/packageProject.cmake/add_package.cmake)
packageProject(
  # the name of the target to export
  NAME ${PROJECT_NAME} # the version of the target to export
  VERSION ${PROJECT_VERSION} # a temporary directory to create the config files
  BINARY_DIR ${PROJECT_BINARY_DIR} # location of the target's public headers
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include # should match the target's INSTALL_INTERFACE include
                                            # directory
  INCLUDE_DESTINATION
    include/${PROJECT_NAME}
    # dir for .cmake files related to scripts for finding targets from the project . We need to
    # change it because it's not multilib and is header-only lib.
    INSTALL_CMAKEDIR
    share/${PROJECT_NAME}/cmake
    # (optional) option to install only header files with matching pattern INCLUDE_HEADER_PATTERN
    # "*.h" semicolon separated list of the project's dependencies DEPENDENCIES "fmt 7.1.3;cxxopts
    # 2.2.0" (optional) create a header containing the version info Note: that the path to headers
    # should be lowercase
  VERSION_HEADER
    "${PROJECT_NAME}/version.h"
    # (optional) create a export header using GenerateExportHeader module
    EXPORT_HEADER
    "${PROJECT_NAME}/export.h"
    # (optional) install your library with a namespace (Note: do NOT add extra '::') NAMESPACE
    # ${PROJECT_NAMESPACE} (optional) define the project's version compatibility, defaults to
    # `AnyNewerVersion` supported values:
    # `AnyNewerVersion|SameMajorVersion|SameMinorVersion|ExactVersion`
  COMPATIBILITY
    AnyNewerVersion
    # (optional) option to disable the versioning of install destinations
    DISABLE_VERSION_SUFFIX
    YES
    # (optional) option to ignore target architecture for package resolution defaults to YES for
    # header only (i.e. INTERFACE) libraries
    ARCH_INDEPENDENT
    YES
    # (optional) option to generate CPack variables
    CPACK
    YES
)

if(${PROJECT_NAME}_INSTALL_MANUALS)
  include(GNUInstallDirs)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/man/ DESTINATION ${CMAKE_INSTALL_MANDIR})
endif()
