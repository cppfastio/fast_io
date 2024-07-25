find_program(Make NAMES make gmake)
if(${Make-NOTFOUND})
  message(WARNING "You have Linux but not make. An example will be skipped for
  building.
  Install make to make the example be build."
  )
else()
  add_custom_command(
    OUTPUT fast_io_module.ko
    COMMAND make
    DEPENDS ${PROJECT_NAME}
    BYPRODUCTS .fast_io_module.mod .kernel.o.cmd fast_io_module.mod kernel.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  )
endif()
