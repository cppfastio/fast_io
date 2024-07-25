# include(${CMAKE_CURRENT_LIST_DIR}/../cmake/get_subdirs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/apply.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/create_exe_default.cmake)

function(apply_to_tests_in_subdir subdir command)
  apply_for_every_compilation_unit_in_subdir(
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${subdir} test_${PROJECT_NAME}
    ${command}
  )
endfunction()

function(create_exe_default_test name source)
  cmake_language(CALL create_exe_default "TESTS" ${name} ${source})
endfunction()

function(fastio_add_test_target name source)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

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
  apply_to_tests_in_subdir(${subdir} create_exe_default_test)
  apply_to_tests_in_subdir(${subdir} fastio_add_test_target)
endforeach()

add_subdirectory(0033.legacy)
