#pragma once

namespace fast_io
{

namespace details
{

template <::std::size_t sz>
inline constexpr ::std::size_t calculate_transmit_buffer_size() noexcept
{
#ifdef FAST_IO_BUFFER_SIZE
	static_assert(sz >= FAST_IO_BUFFER_SIZE);
	static_assert(FAST_IO_BUFFER_SIZE < SIZE_MAX);
	return FAST_IO_BUFFER_SIZE / sz;
#else
	if constexpr (sizeof(::std::size_t) <= sizeof(::std::uint_least16_t))
	{
		return 4096 / sz;
	}
	else
	{
		return 131072 / sz;
	}
#endif
}

template <::std::size_t sz>
inline constexpr ::std::size_t transmit_buffer_size_cache{calculate_transmit_buffer_size<sz>()};

} // namespace details

} // namespace fast_io
