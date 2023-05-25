#pragma once

namespace fast_io::details
{

template<::std::integral T>
inline constexpr
	T read_write_bytes_compute(::std::byte const* first,::std::byte const* last) noexcept
{
	::std::ptrdiff_t sz{last-first};
	constexpr
		auto ul32mx{::std::numeric_limits<T>::max()};
	if constexpr(ul32mx<::std::numeric_limits<::std::ptrdiff_t>::max())
	{
		if(ul32mx<sz)
		{
			sz=ul32mx;
		}
	}
	return static_cast<T>(sz);
}

inline ::std::int_least64_t nt_fpos_offset_addition(::std::uint_least64_t ucurrent,::fast_io::intfpos_t off) noexcept
{
	if constexpr(sizeof(::std::int_least64_t)<sizeof(::fast_io::intfpos_t))
	{
		if(off<INT_LEAST64_MIN)
		{
			return 0;
		}
		else if(INT_LEAST64_MAX<off)
		{
			return -1;
		}
	}
	if(ucurrent>INT_LEAST64_MAX)
	{
		return -1;
	}
	::std::int_least64_t current{static_cast<::std::int_least64_t>(ucurrent)};
	::std::int_least64_t l64off{static_cast<::std::int_least64_t>(off)};
	if(l64off<0)
	{
		::std::uint_least64_t l64abs{
			static_cast<::std::uint_least64_t>
			(static_cast<::std::uint_least64_t>(0)-static_cast<::std::uint_least64_t>(l64off))};
		if(ucurrent<l64abs)
		{
			return 0;
		}
	}
	else
	{
		::std::uint_least64_t l64abs{static_cast<::std::uint_least64_t>(l64off)};
		::std::uint_least64_t l64mx{::std::numeric_limits<::std::int_least64_t>::max()};
		::std::uint_least64_t diff{static_cast<::std::uint_least64_t>(l64mx-l64abs)};
		if(diff<ucurrent)
		{
			return -1;
		}
		return current+l64off;
	}
	return current+l64off;
}

}
