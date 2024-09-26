add_executable(benchmark.0008.comparison ${CMAKE_CURRENT_LIST_DIR}/comparison.cpp)
add_test(benchmark.0008.comparison benchmark.0008.comparison)
include(${CMAKE_CURRENT_LIST_DIR}/seperate/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/seperate_short/CMakeLists.txt)
