# set(CMAKE_CXX_STANDARD 23) add_subdirectory(examples/0001.helloworld)
# add_subdirectory(examples/0002.a+b) add_subdirectory(examples/0003.manipulators)
# add_subdirectory(examples/0004.time) add_subdirectory(examples/0005.sha)
# add_subdirectory(examples/0006.file_io) add_subdirectory(examples/0007.legacy)

set(simple_examples
    0000.compile;
    0001.helloworld;
    0002.a+b;
    0003.manipulators;
    0004.time;
    0005.sha;
    0006.file_io;
    0007.legacy;
    # 0008.openssl;
    0009.filesystem;
    0010.status;
    # 0011.qt;
    0012.transmit;
    0013.graph;
    0014.random;
    0015.match;
    # 0016.dll;
    0017.network;
    0018.i18n;
    0019.llvm;
    # 0020.operating_system; 
    # 0021.kernel_driver;
    0022.complex;
    0023.to;
    0024.mfc;
    # 0025.winrt; 
    # 0026.lua;
    0027.filelock;
    # 0028.python;
    0029.containers;
    # 0030.binary_serialization;
    0031.concat;
    0032.parse_by_scan;
    0033.get_fd;
    # 0033.pulseaudio
)

set(complex_examples
    0008.openssl;
    0011.qt;
    0016.dll;
    0020.operating_system;
    0021.kernel_driver;
    0025.winrt;
    0026.lua;
    0028.python;
    0030.binary_serialization;
    0033.pulseaudio
)

foreach(subdir ${simple_examples})
  file(
    GLOB_RECURSE sources CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}/${subdir}"
    "${CMAKE_CURRENT_LIST_DIR}/${subdir}/**.cc"
  )
  message(DEBUG "sources ${subdir}: ${sources}")
  foreach(source ${sources})
    message(DEBUG "Before: examples_${PROJECT_NAME}_${subdir}_${source}")
    string(REGEX REPLACE "/" "_" name ${source})
    string(REGEX REPLACE "(\.cc|\.cpp|\.cxx)" "" name ${name})
    message(DEBUG "After: examples_${PROJECT_NAME}_${subdir}_${name}")
    add_executable(examples_${PROJECT_NAME}_${subdir}_${name} ${subdir}/${source})
    target_link_libraries(examples_${PROJECT_NAME}_${subdir}_${name} PRIVATE ${PROJECT_NAME})
    set_target_properties(examples_${PROJECT_NAME}_${subdir}_${name} PROPERTIES CXX_STANDARD 20)
    # https://llvm.org/docs/LibFuzzer.html #id24
    target_compile_options(examples_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=fuzzer)
    if(${${PROJECT_NAME}_BUILD_EXAMPLES_ADD_ASAN})
      target_compile_options(examples_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=address)
    endif()
    if(${${PROJECT_NAME}_BUILD_EXAMPLES_ADD_MSAN})
      target_compile_options(examples_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=memory)
    endif()
    if(${${PROJECT_NAME}_BUILD_EXAMPLES_ADD_UBSAN})
      target_compile_options(examples_${PROJECT_NAME}_${subdir}_${name} PRIVATE -fsanitize=undefined)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
       OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin"
       OR CMAKE_SYSTEM_NAME STREQUAL "Msys"
    )
      target_link_libraries(examples_${PROJECT_NAME}_${subdir}_${name} PRIVATE ntdll)
    endif()
  endforeach()
endforeach()
