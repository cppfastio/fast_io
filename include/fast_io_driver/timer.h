#pragma once
#include "../fast_io.h"
#include "../fast_io_dsal/string_view.h"
#include "../fast_io_dsal/impl/misc/push_warnings.h"
#include "../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io
{

namespace details
{

inline ::fast_io::posix_statx_timestamp64 posix_clock_gettime_nothrow_for_timer(::fast_io::posix_clock_id id) noexcept
{
	FAST_IO_HERBCEPTIONS_TRY
	{
		return ::fast_io::posix_clock_gettime(id);
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
		::fast_io::fast_terminate();
	}
}

} // namespace details
struct timer
{
	::fast_io::u8string_view s;
	posix_statx_timestamp64 t0;
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline explicit timer(::fast_io::u8string_view strvw) noexcept
		: s(strvw),
		  t0(::fast_io::details::posix_clock_gettime_nothrow_for_timer(posix_clock_id::monotonic_raw))
	{
	}
	inline timer(timer const &) = delete;
	inline timer &operator=(timer const &) = delete;
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline ~timer()
	{
		FAST_IO_HERBCEPTIONS_TRY
		{
			::fast_io::io::perr(::fast_io::u8err(), s, u8":", posix_clock_gettime(posix_clock_id::monotonic_raw) - t0, u8"s\n");
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{}
	}
};
} // namespace fast_io

#include "../fast_io_dsal/impl/misc/pop_macros.h"
#include "../fast_io_dsal/impl/misc/pop_warnings.h"
