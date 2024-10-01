cmake_minimum_required(VERSION 3.22)
 
project(example CXX)
 
include(FetchContent)
FetchContent_Declare(
        dragonbox
        GIT_REPOSITORY https://github.com/jk-jeon/dragonbox
)
FetchContent_MakeAvailable(dragonbox)
 
add_executable(
    comparison
    ${CMAKE_CURRENT_LIST_DIR}/comparison.cpp
)
target_include_directories(comparison PRIVATE ../include)
target_compile_features(comparison PRIVATE cxx_std_20)
target_link_libraries(comparison dragonbox::dragonbox_to_chars)
