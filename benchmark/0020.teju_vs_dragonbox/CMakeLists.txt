include(FetchContent)

# JKJ Dragonbox
FetchContent_Declare(
  dragonbox
  GIT_REPOSITORY https://github.com/jk-jeon/dragonbox
)

# Teju Jaguá (for to_decimal API)
FetchContent_Declare(
  teju_jagua
  GIT_REPOSITORY https://github.com/cassioneri/teju_jagua
  GIT_TAG        main
)

# Disable testing in all subprojects to avoid "test" target conflicts
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Fetch dragonbox first (no issues)
FetchContent_MakeAvailable(dragonbox)

# For teju_jagua, use a pre-configure hook to patch the conflicting target name
function(patch_teju_test_target)
  FetchContent_GetProperties(teju_jagua)
  if(NOT teju_jagua_POPULATED)
    FetchContent_Populate(teju_jagua)
    
    # Patch the test target name to avoid conflict with CTest
    file(READ ${teju_jagua_SOURCE_DIR}/cpp/test/CMakeLists.txt TEJU_TEST_CMAKE_CONTENT)
    string(REPLACE "add_executable(test" "add_executable(teju_test" TEJU_TEST_CMAKE_CONTENT_PATCHED "${TEJU_TEST_CMAKE_CONTENT}")
    string(REPLACE "target_include_directories(test" "target_include_directories(teju_test" TEJU_TEST_CMAKE_CONTENT_PATCHED "${TEJU_TEST_CMAKE_CONTENT_PATCHED}")
    string(REPLACE "target_link_libraries(test" "target_link_libraries(teju_test" TEJU_TEST_CMAKE_CONTENT_PATCHED "${TEJU_TEST_CMAKE_CONTENT_PATCHED}")
    file(WRITE ${teju_jagua_SOURCE_DIR}/cpp/test/CMakeLists.txt "${TEJU_TEST_CMAKE_CONTENT_PATCHED}")
  endif()
  
  # Now add the subdirectory with the patched content
  add_subdirectory(${teju_jagua_SOURCE_DIR} ${teju_jagua_BINARY_DIR} EXCLUDE_FROM_ALL)
endfunction()

# Apply the patch and build teju_jagua
patch_teju_test_target()

# Fix teju_jagua's include paths for both teju and common targets
# The issue is that teju_jagua's CMakeLists.txt uses ${CMAKE_SOURCE_DIR} which points to fast_io root in our context
# But teju_jagua expects it to point to teju_jagua root
if(TARGET teju)
  # Override the problematic include directory for teju C library
  set_target_properties(teju PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
  target_include_directories(teju PUBLIC
    ${teju_jagua_SOURCE_DIR}/teju/include
  )
  target_include_directories(teju PRIVATE
    ${teju_jagua_SOURCE_DIR}
  )
endif()

if(TARGET common)
  # Override the problematic include directory for cpp/common
  set_target_properties(common PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
  target_include_directories(common PUBLIC
    ${teju_jagua_SOURCE_DIR}/cpp/common/include
  )
  target_include_directories(common PRIVATE
    ${teju_jagua_SOURCE_DIR}
  )
endif()

add_executable(benchmark.0020.teju_vs_dragonbox.teju_vs_dragonbox ${CMAKE_CURRENT_LIST_DIR}/teju_vs_dragonbox.cc)

target_include_directories(benchmark.0020.teju_vs_dragonbox.teju_vs_dragonbox PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${teju_jagua_SOURCE_DIR}
  ${teju_jagua_SOURCE_DIR}/teju/include
  ${teju_jagua_SOURCE_DIR}/cpp/common/include
  ${teju_jagua_SOURCE_DIR}/third-party/dragonbox/include)

target_link_libraries(benchmark.0020.teju_vs_dragonbox.teju_vs_dragonbox PRIVATE
  dragonbox::dragonbox_to_chars
  teju)
