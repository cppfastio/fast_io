# cmake-format: off
set(onefile_test_dirs
    0001.input;
    0002.printscan;
    0003.concat;
    0004.floating;
    0005.lc_time;
    0006.output_device;
    0007.multiparas;
    0008.width;
    0009.http_header;
    0010.intrinsics;
    0011.manipulators;
    0012.umul;
    0013.pr_rsv;
    0014.integer;
    0015.temp_file;
    0016.address_sanitizer;
    0017.error;
    0018.operator_support;
    0019.boost;
    0020.scanners;
    0021.binary_serialization;
    0022.char_categories;
    0023.bitset;
    0024.timestamp;
    0025.ip;
    0026.container;
    0027.rwreflect;
    0028.io_file;
    0029.decorators;
    0030.native_file_loader;
    0031.tpeb;
    0032.process;
    # 0033.legacy
)
# cmake-format: on

foreach(subdir ${onefile_test_dirs})
  file(
    GLOB_RECURSE sources CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}/${subdir}"
    "${CMAKE_CURRENT_LIST_DIR}/${subdir}/**.cc"
  )
  message(DEBUG "sources ${subdir}: ${sources}")
  foreach(source ${sources})
    message(DEBUG "Before: test_${PROJECT_NAME}_${subdir}_${source}")
    string(REGEX REPLACE "/" "_" name ${source})
    string(REGEX REPLACE "(\.cc|\.cpp|\.cxx)" "" name ${name})
    message(DEBUG "After: test_${PROJECT_NAME}_${subdir}_${name}")
    add_executable(test_${PROJECT_NAME}_${subdir}_${name} ${subdir}/${source})
    target_link_libraries(test_${PROJECT_NAME}_${subdir}_${name} PRIVATE ${PROJECT_NAME})
    set_target_properties(test_${PROJECT_NAME}_${subdir}_${name} PROPERTIES CXX_STANDARD 20)
    add_test(NAME test_${PROJECT_NAME}_${subdir}_${name} COMMAND test_${subdir}_${name})
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
       OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin"
       OR CMAKE_SYSTEM_NAME STREQUAL "Msys"
    )
      target_link_libraries(test_${PROJECT_NAME}_${subdir}_${name} PRIVATE ntdll)
    endif()
    if(${${PROJECT_NAME}_ENABLE_TESTS_ADD_ASAN})
      target_compile_options(test_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=address)
    endif()
    if(${${PROJECT_NAME}_ENABLE_TESTS_ADD_MSAN})
      target_compile_options(test_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=memory)
    endif()
    if(${${PROJECT_NAME}_ENABLE_TESTS_ADD_UBSAN})
      target_compile_options(test_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=undefined)
    endif()
  endforeach()
endforeach()

# add_subdirectory(0030.native_file_loader) add_subdirectory(0031.tpeb)
# add_subdirectory(0032.process)
add_subdirectory(0033.legacy)
