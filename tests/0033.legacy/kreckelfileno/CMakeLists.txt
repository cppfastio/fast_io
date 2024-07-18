add_executable(test_${PROJECT_NAME}_0033.legacy_kreckelfileno main.cpp fileno.cpp)
target_link_libraries(test_${PROJECT_NAME}_0033.legacy_kreckelfileno PRIVATE ${PROJECT_NAME})
target_include_directories(test_${PROJECT_NAME}_0033.legacy_kreckelfileno
  PRIVATE ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(test_${PROJECT_NAME}_0033.legacy_kreckelfileno PROPERTIES CXX_STANDARD 20)
add_test(NAME test_${PROJECT_NAME}_0033.legacy_kreckelfileno COMMAND test_${PROJECT_NAME}_0033.legacy_kreckelfileno)
