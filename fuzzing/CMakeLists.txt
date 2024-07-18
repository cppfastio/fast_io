include(${CMAKE_CURRENT_LIST_DIR}/../cmake/get_subdirs.cmake)

subdirlist(subdirs ${CMAKE_CURRENT_LIST_DIR})

message(DEBUG "Fuzzing: subdirs: ${subdirs}")

foreach(subdir ${subdirs})
  file(
    GLOB_RECURSE sources CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}/${subdir}"
    "${CMAKE_CURRENT_LIST_DIR}/${subdir}/**.cc"
  )
  message(DEBUG "sources ${subdir}: ${sources}")
  foreach(source ${sources})
    message(DEBUG "Before: fuzzing_${PROJECT_NAME}_${subdir}_${source}")
    string(REGEX REPLACE "/" "_" name ${source})
    string(REGEX REPLACE "(\.cc|\.cpp|\.cxx)" "" name ${name})
    message(DEBUG "After: fuzzing_${PROJECT_NAME}_${subdir}_${name}")
    add_executable(fuzzing_${PROJECT_NAME}_${subdir}_${name} ${subdir}/${source})
    target_link_libraries(fuzzing_${PROJECT_NAME}_${subdir}_${name} PRIVATE ${PROJECT_NAME})
    set_target_properties(fuzzing_${PROJECT_NAME}_${subdir}_${name} PROPERTIES CXX_STANDARD 20)
    # https://llvm.org/docs/LibFuzzer.html #id24
    target_compile_options(fuzzing_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=fuzzer)
    if(${${PROJECT_NAME}_BUILD_FUZZERS_ADD_ASAN})
      target_compile_options(fuzzing_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=address)
    endif()
    if(${${PROJECT_NAME}_BUILD_FUZZERS_ADD_MSAN})
      target_compile_options(fuzzing_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=memory)
    endif()
    if(${${PROJECT_NAME}_BUILD_FUZZERS_ADD_UBSAN})
      target_compile_options(fuzzing_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=undefined)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
       OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin"
       OR CMAKE_SYSTEM_NAME STREQUAL "Msys"
    )
      target_link_libraries(fuzzing_${PROJECT_NAME}_${subdir}_${name} PRIVATE ntdll)
    endif()
  endforeach()
endforeach()
