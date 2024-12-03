#pragma once
#include "../fast_io.h"
#include "../fast_io_dsal/string_view.h"

namespace fast_io
{

struct timer
{
	::fast_io::u8string_view s;
	unix_timestamp t0;
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline explicit timer(::fast_io::u8string_view strvw)
		: s(strvw), t0(posix_clock_gettime(posix_clock_id::monotonic_raw))
	{
	}
	inline timer(timer const &) = delete;
	inline timer &operator=(timer const &) = delete;
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline ~timer()
	{
		::fast_io::io::perr(::fast_io::u8err(), s, u8":", posix_clock_gettime(posix_clock_id::monotonic_raw) - t0, u8"s\n");
	}
};
} // namespace fast_io
