#pragma once

namespace fast_io
{

#if defined(_MSC_VER) && !defined(_KERNEL_MODE)
#pragma comment(lib,"advapi32.lib")
#endif

template<std::integral ch_type>
class basic_rtl_gen_random
{
public:
	using input_char_type = ch_type;
	constexpr void close() noexcept{}
};

namespace win32
{

namespace details
{

inline ::std::byte* rtl_gen_random_some_impl(::std::byte *first,::std::byte *last)
{
	if constexpr(sizeof(::std::size_t)<=sizeof(::std::uint_least32_t))
	{
		::std::size_t sz{static_cast<::std::size_t>(last-first)};
		if(!::fast_io::win32::SystemFunction036(first,static_cast<std::uint_least32_t>(sz)))
		{
			throw_win32_error();
		}
		return last;
	}
	else
	{
		constexpr std::size_t uintleast32mx{static_cast<std::size_t>(::std::numeric_limits<::std::uint_least32_t>::max())};
		while(first!=last)
		{
			::std::size_t toreadthisround{static_cast<::std::size_t>(last-first)};
			if(uintleast32mx<toreadthisround)
				toreadthisround=uintleast32mx;
			if(!::fast_io::win32::SystemFunction036(first,static_cast<std::uint_least32_t>(toreadthisround)))
			{
				return first;
			}
			first+=toreadthisround;
		}
		return last;
	}
}

inline void rtl_gen_random_all_impl(::std::byte *first,::std::byte *last)
{
	auto ret{rtl_gen_random_some_impl(first,last)};
	if constexpr(sizeof(::std::uint_least32_t)<sizeof(::std::size_t))
	{
		if(ret!=last)
		{
			throw_win32_error();
		}
	}
}

}
}

template<std::integral char_type>
requires (sizeof(::std::uint_least32_t)<sizeof(::std::size_t))
inline ::std::byte* read_some_bytes_underflow_define(basic_rtl_gen_random<char_type>,::std::byte* first,::std::byte* last)
{
	return ::fast_io::win32::details::rtl_gen_random_some_impl(first,last);
}

template<std::integral char_type>
inline void read_all_bytes_underflow_define(basic_rtl_gen_random<char_type>,::std::byte* first,::std::byte* last)
{
	::fast_io::win32::details::rtl_gen_random_all_impl(first,last);
}

template<std::integral char_type>
inline constexpr basic_rtl_gen_random<char_type> input_stream_ref_define(basic_rtl_gen_random<char_type> brg) noexcept
{
	return brg;	
}

using rtl_gen_random = basic_rtl_gen_random<char>;

template<std::integral char_type>
inline constexpr void input_stream_require_secure_clear_define(basic_rtl_gen_random<char_type>) noexcept{}

}