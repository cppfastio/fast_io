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
	using char_type = ch_type;
	constexpr void close() noexcept{}
};

namespace win32
{

namespace details
{
inline void rtl_gen_random_read_u32(void* ptr,std::size_t sz)
{
	if(!SystemFunction036(ptr,static_cast<std::uint_least32_t>(sz)))
		throw_win32_error();
}

inline std::size_t rtl_gen_random_read(void* ptr,std::size_t sz)
{
	std::byte *base_ptr{reinterpret_cast<std::byte*>(ptr)};
	std::byte *iter{base_ptr};
	while(sz)
	{
		constexpr std::size_t uintleast32mx{static_cast<std::size_t>(UINT_LEAST32_MAX)};
		std::size_t mn{sz};
		if(uintleast32mx<sz)
			mn=uintleast32mx;
		if(!SystemFunction036(iter,static_cast<std::uint_least32_t>(mn)))
		{
			if(base_ptr==iter)
				throw_win32_error();
			break;
		}
		sz-=mn;
		iter+=mn;
	}
	return static_cast<std::size_t>(iter-base_ptr);
}
}
}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline Iter read(basic_rtl_gen_random<char_type>,Iter bg,Iter ed)
{
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		auto ret{win32::details::rtl_gen_random_read(::std::to_address(bg),static_cast<std::size_t>(ed-bg)*sizeof(*bg))};
		return bg+(ret/sizeof(*bg));
	}
	else
	{
		win32::details::rtl_gen_random_read_u32(::std::to_address(bg),static_cast<std::size_t>(ed-bg)*sizeof(*bg));
		return ed;
	}
}

template<std::integral char_type>
inline constexpr basic_rtl_gen_random<char_type> io_value_handle(basic_rtl_gen_random<char_type> brg) noexcept
{
	return brg;	
}

using rtl_gen_random = basic_rtl_gen_random<char>;

template<std::integral char_type>
inline constexpr void require_secure_clear(basic_rtl_gen_random<char_type>) noexcept{}

}