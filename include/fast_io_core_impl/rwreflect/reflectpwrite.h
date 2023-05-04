#pragma once

namespace fast_io
{

using uintfpos_t = ::std::uintmax_t;
using intfpos_t = ::std::intmax_t;

namespace details
{

template<typename F>
struct final_guard
{
	F f;
	explicit constexpr final_guard(F ff) noexcept:f{ff}{}

	final_guard(final_guard const&)=delete;
	final_guard& operator=(final_guard const&)=delete;


	constexpr ~final_guard()
	{
		if constexpr(noexcept(f()))
		{
			f();
		}
		else
		{
#ifdef __cpp_exceptions
			try
			{
#endif
				f();
#ifdef __cpp_exceptions
			}
			catch(...){}
#endif
		}
	}
};

template<typename outstmtp,typename Func>
inline constexpr auto seek_op_func(outstmtp outstm,intfpos_t up,Func fn)
{
	intfpos_t pos{seek_define(outstm,up,seekdir::cur)};
	final_guard g([&]()
	{
		seek_define(outstm,pos,seekdir::beg);
	});
	return fn();
}

template<typename F,typename value_type>
inline constexpr value_type const* pwrite_some_common_chtypeptr_impl(F outstm,value_type const* first,value_type const* last,intfpos_t up)
{
	return seek_op_func(outstm,up,[&]()
	{
		return write_some_common_chtypeptr_impl(outstm,first,last);
	});
}

template<typename outputtype,::std::forward_iterator Iter>
inline constexpr Iter pwrite_some_common_iter_impl(outputtype outstm,Iter first,Iter last,intfpos_t up)
{
	using char_type = typename outputtype::char_type;
	using value_type = ::std::iter_value_t<Iter>;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		return ::fast_io::details::pwrite_some_common_chtypeptr_impl(
			outstm,
			::std::to_address(first),
			::std::to_address(last),up)-::std::to_address(first)+first;
	}
	else
	{
		return seek_op_func(outstm,up,[&]
		{
			return write_some_common_iter_impl(outstm,first,last);
		});
	}
}

template<typename F,typename value_type>
inline constexpr void pwrite_all_common_chtypeptr_impl(F outstm,value_type const* first,value_type const* last,intfpos_t up)
{
	seek_op_func(outstm,up,[&]
	{
		write_all_common_iter_impl(outstm,first,last);
	});
}

template<typename outputtype,::std::forward_iterator Iter>
inline constexpr void pwrite_all_common_iter_impl(outputtype outstm,Iter first,Iter last,intfpos_t up)
{
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		::fast_io::details::pwrite_all_common_chtypeptr_impl(
			outstm,::std::to_address(first),::std::to_address(last),up);
	}
	else
	{
		seek_op_func(outstm,up,[&]
		{
			write_all_common_iter_impl(outstm,first,last);
		});
	}
}

template<typename F>
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes_impl(F outstm,io_scatter_t const *base,::std::size_t len,intfpos_t up)
{
	return seek_op_func(outstm,up,[&]
	{
		return scatter_write_some_bytes_impl(outstm,base,len);
	});
}

template<typename F>
inline constexpr void scatter_pwrite_all_bytes_impl(F outstm,io_scatter_t const *base,::std::size_t len,intfpos_t up)
{
	seek_op_func(outstm,up,[&]
	{
		scatter_write_all_bytes_impl(outstm,base,len);
	});
}


template<typename F>
inline constexpr io_scatter_status_t scatter_pwrite_some_impl(F outstm,basic_io_scatter_t<typename F::char_type> const *base,::std::size_t len,intfpos_t up)
{
	return seek_op_func(outstm,up,[&]
	{
		return scatter_write_some_impl(outstm,base,len);
	});
}

template<typename F>
inline constexpr void scatter_pwrite_all_impl(F outstm,basic_io_scatter_t<typename F::char_type> const *base,::std::size_t len,intfpos_t up)
{
	seek_op_func(outstm,up,[&]
	{
		scatter_write_all_impl(outstm,base,len);
	});
}


}

template<typename F,::std::forward_iterator Iter>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr Iter pwrite_some(F&& foo,Iter first, Iter last, intfpos_t up)
{
	return ::fast_io::details::pwrite_some_common_iter_impl(io_ref(foo),first,last,up);
}

template<typename F,::std::forward_iterator Iter>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all(F&& foo,Iter first,Iter last, intfpos_t up)
{
	::fast_io::details::pwrite_all_common_iter_impl(io_ref(foo),first,last,up);
}

template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len,intfpos_t up)
{
	return ::fast_io::details::scatter_pwrite_some_bytes_impl(io_ref(foo),pscatter,len,up);
}

template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pwrite_all_bytes(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len,intfpos_t up)
{
	::fast_io::details::scatter_pwrite_all_bytes_impl(io_ref(foo),pscatter,len,up);
}


template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pwrite_some(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len,intfpos_t up)
{
	return ::fast_io::details::scatter_pwrite_some_impl(io_ref(foo),pscatter,len,up);
}

template<typename F,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pwrite_all(F&& foo,
	basic_io_scatter_t<char_type> const* pscatter,::std::size_t len,intfpos_t up)
{
	::fast_io::details::scatter_pwrite_all_impl(io_ref(foo),pscatter,len,up);
}

}
