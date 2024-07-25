include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/packages/FindWDK/add_findwdk.cmake)

list(APPEND CMAKE_MODULE_PATH "${FindWDK_SOURCE_DIR}")
find_package(WDK REQUIRED)
wdk_add_driver(
  example_${PROJECT_NAME}_0021.kernel_driver_nt_kernel_entry
  # KMDF 1.15 WINVER 0x0602
  kernel_entry.cc
)
target_link_libraries(
  example_${PROJECT_NAME}_0021.kernel_driver_nt_kernel_entry PRIVATE ${PROJECT_NAME}
)
