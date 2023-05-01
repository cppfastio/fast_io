#pragma once
#include<string_view>

namespace fast_io
{

struct timer
{
	std::u8string_view s;
	unix_timestamp t0;
	#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
	#endif
	explicit timer(std::u8string_view strvw):s(strvw),t0(posix_clock_gettime(posix_clock_id::monotonic_raw)){}
	timer(timer const &)=delete;
	timer& operator=(timer const &)=delete;
	#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
	#endif
	~timer()
	{
		::fast_io::io::perr(fast_io::u8err(),s, u8":", posix_clock_gettime(posix_clock_id::monotonic_raw)-t0,u8"s\n");
	}
};
}
