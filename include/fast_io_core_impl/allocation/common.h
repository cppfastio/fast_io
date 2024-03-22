#pragma once

namespace fast_io
{

template <typename Pointer>
struct basic_allocation_least_result
{
	using pointer = Pointer;
	using size_type = ::std::size_t;
	pointer ptr;
	size_type count;
};

using allocation_least_result = basic_allocation_least_result<void*>;

} // namespace fast_io
