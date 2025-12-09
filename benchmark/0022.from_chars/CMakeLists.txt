include(FetchContent)

FetchContent_Declare(
  fast_float
  GIT_REPOSITORY https://github.com/fastfloat/fast_float
  GIT_TAG        main
)
FetchContent_MakeAvailable(fast_float)


add_executable(benchmark.0022.from_chars ${CMAKE_CURRENT_LIST_DIR}/atoi_vs_from_chars.cc)
target_include_directories(benchmark.0022.from_chars PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(benchmark.0022.from_chars PRIVATE cxx_std_20)

# Prefer official target if provided by fast_float; otherwise include headers directly
if (TARGET fast_float::fast_float)
  target_link_libraries(benchmark.0022.from_chars PRIVATE fast_float::fast_float)
else()
  FetchContent_GetProperties(fast_float)
  if (fast_float_SOURCE_DIR)
    target_include_directories(benchmark.0022.from_chars PRIVATE ${fast_float_SOURCE_DIR}/include)
  endif()
endif()

add_executable(benchmark.0022.from_chars_hex ${CMAKE_CURRENT_LIST_DIR}/atoi_vs_from_chars_hex.cc)
target_include_directories(benchmark.0022.from_chars_hex PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(benchmark.0022.from_chars_hex PRIVATE cxx_std_20)

if (TARGET fast_float::fast_float)
  target_link_libraries(benchmark.0022.from_chars_hex PRIVATE fast_float::fast_float)
else()
  FetchContent_GetProperties(fast_float)
  if (fast_float_SOURCE_DIR)
    target_include_directories(benchmark.0022.from_chars_hex PRIVATE ${fast_float_SOURCE_DIR}/include)
  endif()
endif()
