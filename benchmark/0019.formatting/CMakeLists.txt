cmake_minimum_required(VERSION 3.22)
project(benchmark_formatting CXX)

include(FetchContent)

# Fetch fmt library
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        10.2.1
)
FetchContent_MakeAvailable(fmt)

add_executable(benchmark.0019.formatting.format_vs_fmt ${CMAKE_CURRENT_LIST_DIR}/format_vs_fmt.cc)
target_include_directories(benchmark.0019.formatting.format_vs_fmt PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(benchmark.0019.formatting.format_vs_fmt PRIVATE cxx_std_20)
target_link_libraries(benchmark.0019.formatting.format_vs_fmt PRIVATE fmt::fmt)
