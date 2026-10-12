#pragma once

namespace fast_io
{
namespace details
{

inline ::std::byte *wasi_random_get_some_impl(::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using u8ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::std::uint_least8_t *;
	if constexpr (sizeof(::std::size_t) <= sizeof(__wasi_size_t))
	{
		::std::size_t sz{count};
		auto ret{::fast_io::noexcept_call(__wasi_random_get, reinterpret_cast<u8ptr_may_alias>(first),
										  static_cast<__wasi_size_t>(sz))};
		if (ret)
		{
			throw_posix_error(ret);
		}
		return first + count;
	}
	else
	{
		constexpr ::std::size_t uintleast32mx{static_cast<::std::size_t>(::std::numeric_limits<__wasi_size_t>::max())};
		auto const e{first + count};
		while (first != e)
		{
			::std::size_t toreadthisround{static_cast<::std::size_t>(e - first)};
			if (uintleast32mx < toreadthisround)
			{
				toreadthisround = uintleast32mx;
			}
			auto ret{::fast_io::noexcept_call(__wasi_random_get, reinterpret_cast<u8ptr_may_alias>(first),
											  static_cast<__wasi_size_t>(toreadthisround))};
			if (ret)
			{
				throw_posix_error(ret);
			}
			first += toreadthisround;
		}
		return first + count;
	}
}

inline void wasi_random_get_all_impl(::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof(::std::size_t) <= sizeof(__wasi_size_t))
	{
		(void)wasi_random_get_some_impl(first, count);
	}
	else
	{
		using u8ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= ::std::uint_least8_t *;
		constexpr ::std::size_t uintleast32mx{static_cast<::std::size_t>(::std::numeric_limits<__wasi_size_t>::max())};
		auto const e{first + count};
		while (first != e)
		{
			::std::size_t toreadthisround{static_cast<::std::size_t>(e - first)};
			if (uintleast32mx < toreadthisround)
			{
				toreadthisround = uintleast32mx;
			}
			auto ret{::fast_io::noexcept_call(__wasi_random_get, reinterpret_cast<u8ptr_may_alias>(first),
											  static_cast<__wasi_size_t>(toreadthisround))};
			if (ret)
			{
				throw_posix_error(ret);
			}
			first += toreadthisround;
		}
	}
}

} // namespace details

template <::std::integral ch_type>
class basic_wasi_random_get
{
public:
	using input_char_type = ch_type;
	inline constexpr void close() noexcept
	{}
};

template <::std::integral char_type>
	requires(sizeof(__wasi_size_t) < sizeof(::std::size_t))
inline ::std::byte *read_some_bytes_underflow_define(basic_wasi_random_get<char_type>, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::wasi_random_get_some_impl(first, count);
}

template <::std::integral char_type>
inline void read_all_bytes_underflow_define(basic_wasi_random_get<char_type>, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::wasi_random_get_all_impl(first, count);
}

template <::std::integral char_type>
inline constexpr basic_wasi_random_get<char_type> input_stream_ref_define(basic_wasi_random_get<char_type> brg) noexcept
{
	return brg;
}

using wasi_random_get = basic_wasi_random_get<char>;

template <::std::integral char_type>
inline constexpr void input_stream_require_secure_clear_define(basic_wasi_random_get<char_type>) noexcept
{
}

} // namespace fast_io
