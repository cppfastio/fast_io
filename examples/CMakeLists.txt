include(${CMAKE_CURRENT_LIST_DIR}/../cmake/apply.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/create_exe_default.cmake)

function(apply_to_examples_in_subdir subdir command)
  apply_for_every_compilation_unit_in_subdir(
    ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR} ${subdir} example_${PROJECT_NAME}
    ${command}
  )
endfunction()

function(apply_to_sources_examples sources_list_name command)
  apply_for_every_compilation_unit(${sources_list_name} example_${PROJECT_NAME} ${command})
endfunction()

function(create_exe_default_example name source)
  cmake_language(CALL create_exe_default "EXAMPLES" ${name} ${source})
endfunction()

# cmake-format: off
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
    # 0016.dll; # idk where "a.dll"
    0017.network;
    # 0018.i18n;
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

set(examples_complicated
  # 0020.operating_system; # There's no cmake # TODO
  0021.kernel_driver;
  0030.binary_serialization;
)

set(examples_openssl 
    0008.openssl;
)

set(examples_qt 
    0011.qt;
)

set(examples_windows
    0016.dll; # idk where "a.dll"
)

set(examples_winrt
    0025.winrt
)

set(examples_lua
    0026.lua;
)

set(examples_python
    0028.python
)

set(examples_pulseaudio
    0033.pulseaudio
)

# cmake-format: on

set(examples_to_compile "")
list(APPEND examples_to_compile ${simple_examples})

find_package(OpenSSL COMPONENTS Crypto SSL)
if(${OPENSSL_FOUND})
  list(APPEND examples_to_compile ${examples_openssl})
endif()

find_package(Qt5 COMPONENTS Core)
find_package(Qt6 COMPONENTS Core)
if(${QT5_FOUND} OR ${QT6_FOUND})
  list(APPEND examples_to_compile ${examples_qt})
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  list(APPEND examples_to_compile ${examples_windows})
  list(APPEND examples_to_compile ${examples_winrt})
endif()

find_package(Lua)
if(${LUA_FOUND})
  list(APPEND examples_to_compile ${examples_lua})
endif()

find_package(Python COMPONENTS Development)
if(${Python_Development_FOUND})
  list(APPEND examples_to_compile ${examples_python})
endif()

find_package(PulseAudio)
if(${PulseAudio_FOUND})
  list(APPEND examples_to_compile ${examples_pulseaudio})
endif()

foreach(subdir ${examples_to_compile})
  apply_to_examples_in_subdir(${subdir} create_exe_default_example)
endforeach()

function(link_openssl name source)
  target_link_libraries(${name} PRIVATE OpenSSL::Crypto OpenSSL::SSL)
  if(WIN32)
    target_link_libraries(${name} PRIVATE OpenSSL::applink)
  endif()
endfunction()

if(${OPENSSL_FOUND})
  foreach(subdir ${examples_openssl})
    apply_to_examples_in_subdir(${subdir} link_openssl)
  endforeach()
endif()

function(link_qt_core name source)
  if(${QT5_FOUND})
    target_link_libraries(${name} PRIVATE Qt5::Core)
  else()
    target_link_libraries(${name} PRIVATE Qt6::Core)
  endif()
endfunction()

if(${QT5_FOUND} OR ${QT6_FOUND})
  foreach(subdir ${examples_qt})
    apply_to_examples_in_subdir(${subdir} link_qt_core)
  endforeach()
endif()

function(link_winrt name source)
  set_target_properties(${NAME} PROPERTIES VS_WINRT_COMPONENT TRUE)
endfunction()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  foreach(subdir ${examples_winrt})
    apply_to_examples_in_subdir(${subdir} link_winrt)
  endforeach()
endif()

function(link_lua name source)
  target_link_libraries(${name} PRIVATE ${LUA_LIBRARIES})
  target_include_directories(${name} PRIVATE ${LUA_INCLUDE_DIR})
endfunction()

if(${LUA_FOUND})
  foreach(subdir ${examples_lua})
    apply_to_examples_in_subdir(${subdir} link_lua)
  endforeach()
endif()

function(link_python name source)
  target_link_libraries(${name} PRIVATE Python::Module)
  target_include_directories(${name} PRIVATE ${Python_INCLUDE_DIR})
  target_link_options(${name} PRIVATE ${Python_LINK_OPTIONS})
endfunction()

if(${Python_Development_FOUND})
  foreach(subdir ${examples_python})
    apply_to_examples_in_subdir(${subdir} link_python)
  endforeach()
endif()

function(link_pulseaudio name source)
  target_link_libraries(${name} PRIVATE ${PulseAudio_LIBRARIES} ${PulseAudio_MAINLOOP_LIBRARY})
  target_include_directories(${name} PRIVATE ${PulseAudio_INCLUDE_DIRS})
endfunction()

if(${PulseAudio_FOUND})
  foreach(subdir ${examples_pulseaudio})
    apply_to_examples_in_subdir(${subdir} link_pulseaudio)
  endforeach()
endif()

foreach(subdir ${examples_complicated})
  add_subdirectory(${subdir})
endforeach()
