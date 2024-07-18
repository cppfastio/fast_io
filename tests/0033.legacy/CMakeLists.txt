add_executable(test_${PROJECT_NAME}_0033.legacy_getfd getfd.cc)
target_link_libraries(test_${PROJECT_NAME}_0033.legacy_getfd PRIVATE ${PROJECT_NAME})
set_target_properties(test_${PROJECT_NAME}_0033.legacy_getfd PROPERTIES CXX_STANDARD 20)
add_test(NAME test_${PROJECT_NAME}_0033.legacy_getfd COMMAND test_${PROJECT_NAME}_0033.legacy_getfd)

add_subdirectory(kreckelfileno)
