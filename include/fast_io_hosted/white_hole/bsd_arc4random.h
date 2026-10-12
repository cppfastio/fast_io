#pragma once

namespace fast_io
{

template <::std::integral char_type>
struct basic_bsd_arc4random
{
public:
	using input_char_type = char_type;
	inline constexpr void close() noexcept
	{}
};

template <::std::integral char_type>
inline constexpr basic_bsd_arc4random<char_type> input_stream_ref_define(basic_bsd_arc4random<char_type> barc4) noexcept
{
	return barc4;
}

using bsd_arc4random = basic_bsd_arc4random<char>;

template <::std::integral char_type>
inline constexpr void input_stream_require_secure_clear_define(basic_bsd_arc4random<char_type>) noexcept
{
}

#if !defined(__GLIBC__) || (defined(__GLIBC__) && __GLIBC_PREREQ(2, 36))
namespace details
{

inline void bsd_arc4random_read_all_bytes_define_impl(::std::byte *first, ::std::size_t count) noexcept
{
	::fast_io::noexcept_call(arc4random_buf, first, count);
}

} // namespace details

template <::std::integral char_type>
inline void read_all_bytes_underflow_define(basic_bsd_arc4random<char_type>, ::std::byte *first, ::std::size_t count) noexcept
{
	::fast_io::details::bsd_arc4random_read_all_bytes_define_impl(first, count);
}

#else

namespace details
{

inline ::std::byte *posix_getrandom_read_some_bytes_define_impl(unsigned flags, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t sz{count};

	auto ret{noexcept_call(::getrandom, first, sz, flags)};
	if (ret < 0)
	{
		throw_posix_error();
	}

	return first + ret;
}

[[__gnu__::__weak__]]
extern void glibc_arc4random_buf(void *, size_t) noexcept __asm__("arc4random_buf");

inline ::std::byte *bsd_arc4random_read_some_bytes_define_impl(::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr auto *glibc_arc4random_bufptr{::fast_io::details::glibc_arc4random_buf};
	if (glibc_arc4random_bufptr == nullptr)
	{
		return posix_getrandom_read_some_bytes_define_impl(0, first, count);
	}
	else
	{
		glibc_arc4random_bufptr(first, count);
		return first + count;
	}
}
} // namespace details

template <::std::integral char_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_bsd_arc4random<char_type>, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::bsd_arc4random_read_some_bytes_define_impl(first, count);
}

#endif

} // namespace fast_io
