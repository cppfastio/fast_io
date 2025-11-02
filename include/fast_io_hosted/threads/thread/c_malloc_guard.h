#pragma once

namespace fast_io::details
{
class thread_start_routine_tuple_c_malloc_allocate_guard
{
public:
	void *ptr_{nullptr};

	constexpr thread_start_routine_tuple_c_malloc_allocate_guard() = default;

	constexpr thread_start_routine_tuple_c_malloc_allocate_guard(void *ptr) : ptr_{ptr}
	{}

	constexpr thread_start_routine_tuple_c_malloc_allocate_guard(thread_start_routine_tuple_c_malloc_allocate_guard const &) noexcept = delete;
	constexpr thread_start_routine_tuple_c_malloc_allocate_guard(thread_start_routine_tuple_c_malloc_allocate_guard &&other) noexcept = default;

	constexpr ~thread_start_routine_tuple_c_malloc_allocate_guard()
	{
		if (ptr_ != nullptr)
		{
			::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>::deallocate(this->ptr_);
		}
	}
};
} // namespace fast_io::details
