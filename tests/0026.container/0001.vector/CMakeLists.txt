add_executable(emplace_index emplace_index.cc)
add_test(vector_emplace_index emplace_index)

add_executable(emplace emplace.cc)
add_test(vector_emplace emplace)


add_executable(shared_ptr shared_ptr.cc)
add_test(vector_shared_ptr_emplace shared_ptr)

add_executable(shared_ptr_index shared_ptr_index.cc)
add_test(vector_shared_ptr_index shared_ptr_index)