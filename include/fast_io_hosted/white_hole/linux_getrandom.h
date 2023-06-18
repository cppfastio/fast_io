#pragma once
#if !(defined(__linux__) && defined(__NR_getrandom)) && __has_include(<sys/random.h>)
#include<sys/random.h>
#endif

namespace fast_io
{

template<std::integral char_type>
class basic_linux_getrandom
{
public:
	using input_char_type = char_type;
	unsigned flags{};
	constexpr void close() noexcept{}
};

template<std::integral char_type>
inline constexpr basic_linux_getrandom<char_type> input_stream_ref_define(basic_linux_getrandom<char_type> brg) noexcept
{
	return brg;	
}

namespace details
{
inline ::std::byte* linux_getrandom_read_some_bytes_define_impl(unsigned flags,::std::byte *first,::std::byte *last)
{
	::std::size_t sz{static_cast<::std::size_t>(last-first)};
#if defined(__linux__) && defined(__NR_getrandom)
	auto ret{system_call<__NR_getrandom,std::ptrdiff_t>(first,sz,flags)};
	system_call_throw_error(ret);
#else
	auto ret{noexcept_call(getrandom,first,sz,flags)};
	if(ret<0)
		throw_posix_error();
#endif
	return first+ret;
}
}

template<std::integral char_type>
inline ::std::byte* read_some_bytes_underflow_define(basic_linux_getrandom<char_type> g,::std::byte* first,::std::byte* last)
{
	return ::fast_io::details::linux_getrandom_read_some_bytes_define_impl(g.flags,first,last);
}

using linux_getrandom = basic_linux_getrandom<char>;

template<std::integral char_type>
inline constexpr void input_stream_require_secure_clear_define(basic_linux_getrandom<char_type>) noexcept{}

}
