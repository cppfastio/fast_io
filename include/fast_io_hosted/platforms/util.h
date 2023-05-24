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

}
