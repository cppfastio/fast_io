#pragma once

namespace fast_io
{

template <typename Pointer>
struct allocation_least_result
{
	using pointer = Pointer;
	using size_type = ::std::size_t;
	pointer ptr;
	size_type count;
};

} // namespace fast_io