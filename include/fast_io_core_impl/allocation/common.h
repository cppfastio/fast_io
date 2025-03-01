#pragma once

#include <cstdint>

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

using allocation_least_result = basic_allocation_least_result<void *>;

namespace win32::nt
{

struct unicode_string
{
	::std::uint_least16_t Length;
	::std::uint_least16_t MaximumLength;
	char16_t *Buffer;
};

struct client_id
{
	void *UniqueProcess;
	void *UniqueThread;
};

} // namespace win32::nt
} // namespace fast_io
