#pragma once

namespace fast_io
{
namespace details
{

inline void wasi_random_get_full_impl(void* ptr,std::size_t sz)
{
	using u8ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= std::uint_least8_t*;
	auto ret{::fast_io::noexcept_call(__wasi_random_get,reinterpret_cast<u8ptr_may_alias>(ptr),static_cast<__wasi_size_t>(sz))};
	if(ret)
		throw_posix_error(ret);
}

inline std::size_t wasi_random_get_impl(void* ptr,std::size_t sz)
{
	using u8ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= std::uint_least8_t*;
	static_assert(std::unsigned_integral<__wasi_size_t>);
	u8ptr_may_alias base_ptr{reinterpret_cast<u8ptr_may_alias>(ptr)};
	u8ptr_may_alias iter{base_ptr};
	while(sz)
	{
		constexpr std::size_t sz_max{static_cast<std::size_t>(SIZE_MAX)};
		__wasi_size_t mn{sz};
		if(sz_max<sz)
			mn=sz_max;
		auto ret{::fast_io::noexcept_call(__wasi_random_get,iter,mn)};
		if(ret)
		{
			if(base_ptr==iter)
				throw_posix_error(ret);
			break;
		}
		sz-=mn;
		iter+=mn;
	}
	return static_cast<std::size_t>(iter-base_ptr);
}
}

template<std::integral ch_type>
class basic_wasi_random_get
{
public:
	using char_type = ch_type;
	constexpr void close() noexcept{}
};

template<std::integral char_type,::std::contiguous_iterator Iter>
inline Iter read(basic_wasi_random_get<char_type>,Iter bg,Iter ed)
{
	if constexpr(sizeof(__wasi_size_t)<sizeof(std::size_t))
	{
		std::size_t sz{::fast_io::details::wasi_random_get_impl(::std::to_address(bg),static_cast<std::size_t>(ed-bg)*sizeof(*bg))};
		return bg+(sz/sizeof(*bg));
	}
	else
	{
		::fast_io::details::wasi_random_get_full_impl(::std::to_address(bg),static_cast<std::size_t>(ed-bg)*sizeof(*bg));
		return ed;
	}
}

template<std::integral char_type>
inline constexpr basic_wasi_random_get<char_type> io_value_handle(basic_wasi_random_get<char_type> brg) noexcept
{
	return brg;	
}

using wasi_random_get = basic_wasi_random_get<char>;

template<std::integral char_type>
inline constexpr void require_secure_clear(basic_wasi_random_get<char_type>){}

}