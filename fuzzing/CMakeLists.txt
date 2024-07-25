include(${CMAKE_CURRENT_LIST_DIR}/../cmake/get_subdirs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/apply.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/create_exe_default.cmake)

function(apply_to_fuzzing_in_subdir subdir command)
  apply_for_every_compilation_unit_in_subdir(
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${subdir} fuzzing_${PROJECT_NAME}
    ${command}
  )
endfunction()

function(create_exe_default_fuzzing name source)
  cmake_language(CALL create_exe_default "FUZZERS" ${name} ${source})
endfunction()

function(add_fuzzing_flags_libfuzzer name source)
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
  target_link_options(${name} PRIVATE -fsanitize=fuzzer)
endfunction()

subdirlist(subdirs ${CMAKE_CURRENT_SOURCE_DIR})
message(DEBUG "Fuzzing: subdirs: ${subdirs}")

foreach(subdir ${subdirs})
  apply_to_fuzzing_in_subdir(${subdir} create_exe_default_fuzzing)
  apply_to_fuzzing_in_subdir(${subdir} add_fuzzing_flags_libfuzzer)
endforeach()
