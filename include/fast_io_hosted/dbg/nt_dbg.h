#pragma once

namespace fast_io
{

template<std::integral ch_type>
struct basic_nt_dbg
{
	using char_type = ch_type;
	std::uint_least32_t component_id{UINT_LEAST32_MAX};
	std::uint_least32_t level{};
	static inline constexpr std::size_t output_buffer_alignment_size{512u};
};

template<std::integral char_type>
inline constexpr basic_nt_dbg<char_type> io_value_handle(basic_nt_dbg<char_type> serv) noexcept
{
	return {serv};
}

using nt_dbg = basic_nt_dbg<char>;
using wnt_dbg = basic_nt_dbg<wchar_t>;
using u8nt_dbg = basic_nt_dbg<char8_t>;
using u16nt_dbg = basic_nt_dbg<char16_t>;
using u32nt_dbg = basic_nt_dbg<char32_t>;

namespace details
{

template<bool iswide,std::size_t n>
inline constexpr std::size_t calculate_nt_dbg_fmt_string_size() noexcept
{
	if constexpr(iswide)
	{
		static_assert((SIZE_MAX-1/3)>n);
		return 3*n+1;//%wZ
	}
	else
	{
		static_assert((SIZE_MAX-1/2)>n);
		return 2*n+1;//%Z
	}
}

template<bool iswide,std::size_t n>
inline constexpr auto calculate_nt_dbg_fmt() noexcept
{
	::fast_io::freestanding::array<char8_t,calculate_nt_dbg_fmt_string_size<iswide,n>()> fmt_arr;
	for(std::size_t i{};i!=n;++i)
	{
		if constexpr(iswide)
		{
			std::size_t threei{i*3};
			fmt_arr[threei]=u8'%';
			fmt_arr[threei+1]=u8'w';
			fmt_arr[threei+2]=u8'Z';
		}
		else
		{
			std::size_t threei{i*2};
			fmt_arr[threei]=u8'%';
			fmt_arr[threei+1]=u8'Z';
		}
	}
	fmt_arr.back()=0;
	return fmt_arr;
}

template<bool iswide,std::size_t n>
inline constexpr auto nt_fmt_cache{calculate_nt_dbg_fmt<iswide,n>()};

struct nt_dbg_carrier
{
	std::uint_least32_t component_id{UINT_LEAST32_MAX};
	std::uint_least32_t level{};
};

template<std::size_t n,typename... Args>
inline void nt_fmt_dbg_forward([[maybe_unused]] nt_dbg_carrier carr,char8_t const* fmt,io_scatter_t const* arg,Args const* ... args) noexcept
{
	if constexpr(n==0)
	{
#if _WIN32_WINNT >= 0x0501
		::fast_io::win32::nt::DbgPrintEx(carr.component_id,carr.level,reinterpret_cast<char const*>(fmt),args...);
#else
		::fast_io::win32::nt::DbgPrint(reinterpret_cast<char const*>(fmt),args...);
#endif
	}
	else
	{
		--arg;
		void const* arg_n_ptr{arg->base};
		std::size_t arg_n_len{arg->len};
		if(UINT_LEAST16_MAX<arg_n_len)
		{
			arg_n_len=UINT_LEAST16_MAX;
		}
		std::uint_least16_t const u16argnlen{static_cast<std::uint_least16_t>(arg_n_len)};
		::fast_io::win32::nt::ansi_string astr{u16argnlen,u16argnlen,const_cast<char*>(reinterpret_cast<char const*>(arg_n_ptr))};
		nt_fmt_dbg_forward<n-1>(carr,fmt,arg,__builtin_addressof(astr),args...);
	}
}

template<bool iswide>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void nt_dbg_write_impl([[maybe_unused]] nt_dbg_carrier carr,char const* first,char const* last) noexcept
{
	std::size_t arg_n_len{static_cast<std::size_t>(last-first)};
	if(UINT_LEAST16_MAX<arg_n_len)
	{
		arg_n_len=UINT_LEAST16_MAX;
	}
	std::uint_least16_t const u16argnlen{static_cast<std::uint_least16_t>(arg_n_len)};
	::fast_io::win32::nt::ansi_string astr{u16argnlen,u16argnlen,const_cast<char*>(first)};
#if _WIN32_WINNT >= 0x0501
	::fast_io::win32::nt::DbgPrintEx(carr.component_id,carr.level,
		reinterpret_cast<char const*>(nt_fmt_cache<iswide,1>.element),
		__builtin_addressof(astr));
#else
	::fast_io::win32::nt::DbgPrint(reinterpret_cast<char const*>(nt_fmt_cache<iswide,1>.element),__builtin_addressof(astr));
#endif

}

template<bool iswide,std::size_t n>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void nt_dbg_scatter_constant_write_impl(nt_dbg_carrier carr,io_scatter_t const* scatters) noexcept
{
	if constexpr(n==0)
	{
		return;
	}
	else
	{
		nt_fmt_dbg_forward<n>(carr,nt_fmt_cache<iswide,n>.element,scatters+n);
	}
}

}

template<std::integral ch_type,::std::contiguous_iterator Iter>
inline void write(basic_nt_dbg<ch_type> d,Iter first,Iter last) noexcept
{
	constexpr bool is_wide{sizeof(ch_type)==2&&std::same_as<::std::iter_value_t<Iter>,ch_type>};
	::fast_io::details::nt_dbg_write_impl<is_wide>({d.component_id,d.level},
		reinterpret_cast<char const*>(::std::to_address(first)),
		reinterpret_cast<char const*>(::std::to_address(last)));
}

template<std::size_t n,std::integral ch_type>
inline void scatter_constant_write(basic_nt_dbg<ch_type> d,io_scatter_t const* pscatters) noexcept
{
	constexpr bool is_wide{sizeof(ch_type)==2};
	::fast_io::details::nt_dbg_scatter_constant_write_impl<is_wide,n>({d.component_id,d.level},pscatters);
}

#if !defined(_WIN32_WINDOWS)
inline auto dbg(std::uint_least32_t component_id=UINT_LEAST32_MAX,std::uint_least32_t level=0) noexcept
{
	return nt_dbg{component_id,level};
}
inline auto wdbg(std::uint_least32_t component_id=UINT_LEAST32_MAX,std::uint_least32_t level=0) noexcept
{
	return wnt_dbg{component_id,level};
}
inline auto u8dbg(std::uint_least32_t component_id=UINT_LEAST32_MAX,std::uint_least32_t level=0) noexcept
{
	return u8nt_dbg{component_id,level};
}
inline auto u16dbg(std::uint_least32_t component_id=UINT_LEAST32_MAX,std::uint_least32_t level=0) noexcept
{
	return u16nt_dbg{component_id,level};
}
inline auto u32dbg(std::uint_least32_t component_id=UINT_LEAST32_MAX,std::uint_least32_t level=0) noexcept
{
	return u32nt_dbg{component_id,level};
}
#endif

}
