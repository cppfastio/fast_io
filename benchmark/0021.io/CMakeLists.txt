add_executable(benchmark.0021.io.file_vs_stdio ${CMAKE_CURRENT_LIST_DIR}/file_vs_stdio.cc)
target_include_directories(benchmark.0021.io.file_vs_stdio PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(benchmark.0021.io.file_vs_stdio PRIVATE cxx_std_20)
