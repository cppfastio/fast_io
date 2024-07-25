if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_subdirectory(linux)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
   OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin"
   OR CMAKE_SYSTEM_NAME STREQUAL "Msys"
)
  add_subdirectory(nt)
endif()
