#pragma once

namespace fast_io
{

template<typename T>
inline constexpr io_scatter_status_t scatter_size_to_status(std::size_t sz,basic_io_scatter_t<T> const *base,::std::size_t len) noexcept
{
	std::size_t total{sz};
	for(std::size_t i{};i!=len;++i)
	{
		::std::size_t blen{base[i].len};
		if(total<blen)[[unlikely]]
			return {i,total};
		total-=blen;
	}
	return {len,0};
}

namespace details
{
template<typename T>
struct basic_scatter_total_size_overflow_result
{
	T total_size{};
	::std::size_t position{};
};
}
using scatter_total_size_overflow_result = ::fast_io::details::basic_scatter_total_size_overflow_result<::std::size_t>;

namespace details
{

template<::std::unsigned_integral U,typename T>
inline constexpr ::fast_io::details::basic_scatter_total_size_overflow_result<U> find_scatter_total_size_overflow_impl(basic_io_scatter_t<T> const *base,U len) noexcept
{
	constexpr
		U mx{static_cast<::std::size_t>(::std::numeric_limits<::std::make_signed_t<U>>::max())};
	U total{};
	auto i{base},e{base+len};
	for(;i!=e;++i)
	{
		if(total<static_cast<U>(static_cast<::std::size_t>(mx-i->len)))
		{
			break;
		}
	}
	return {total,static_cast<::std::size_t>(i-base)};
}

}

template<typename T>
inline constexpr scatter_total_size_overflow_result find_scatter_total_size_overflow(basic_io_scatter_t<T> const *base,::std::size_t len) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::details::find_scatter_total_size_overflow_impl<::std::size_t>(base,len);
	}
	else
	{
using io_scatter_alias_ptr

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
= io_scatter_t const*;
		return ::fast_io::details::find_scatter_total_size_overflow_impl<::std::size_t>(reinterpret_cast<io_scatter_alias_ptr>(base),len);
	}
}

namespace details
{
inline constexpr ::std::size_t scatter_status_one_size_impl(::std::size_t position,::std::size_t position_in_scatter,::std::size_t n) noexcept
{
	if(position)
	{
		return n;
	}
	return position_in_scatter;
}
}

inline constexpr ::std::size_t scatter_status_one_size(io_scatter_status_t status,::std::size_t n) noexcept
{
	return ::fast_io::details::scatter_status_one_size_impl(status.position,status.position_in_scatter,n);
}

template<::std::integral dftype>
inline constexpr ::fast_io::intfpos_t fposoffadd_nonegative(::fast_io::intfpos_t off,dftype df) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(0<=df)]];
#endif

	constexpr
		::fast_io::intfpos_t mxv{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	constexpr
		::fast_io::uintfpos_t umxv{static_cast<::fast_io::uintfpos_t>(mxv)};
	if constexpr(mxv<::std::numeric_limits<dftype>::max())
	{
		if(mxv<df)
		{
			return mxv;
		}
	}
	::fast_io::intfpos_t mx{static_cast<::fast_io::intfpos_t>(umxv-static_cast<::fast_io::uintfpos_t>(df))};
	if(mx<off)
	{
		return mxv;
	}
	else
	{
		return off+static_cast<::fast_io::intfpos_t>(df);
	}
}

template<::std::integral dftype>
inline constexpr ::fast_io::intfpos_t fposoffadd(::fast_io::intfpos_t off,dftype df) noexcept
{
	constexpr
		::fast_io::intfpos_t mxv{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	constexpr
		::fast_io::uintfpos_t umxv{static_cast<::fast_io::uintfpos_t>(mxv)};
	if constexpr(mxv<::std::numeric_limits<dftype>::max())
	{
		if(mxv<df)
		{
			return mxv;
		}
	}
	if constexpr(::std::signed_integral<dftype>)
	{
		if(df<0)
		{
			constexpr
				::fast_io::intfpos_t mnv{::std::numeric_limits<::fast_io::intfpos_t>::min()};
			if constexpr(::std::numeric_limits<dftype>::min()<mxv)
			{
				if(df<mxv)
				{
					return mnv;
				}
			}
			auto ddf{mnv-df};
			if(off<ddf)
			{
				return mnv;
			}
			else
			{
				return off+df;
			}
		}
	}
	::fast_io::intfpos_t mx{static_cast<::fast_io::intfpos_t>(umxv-static_cast<::fast_io::uintfpos_t>(df))};
	if(mx<off)
	{
		return mxv;
	}
	else
	{
		return off+df;
	}
}

namespace details
{

template<typename T>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::fast_io::intfpos_t fposoffadd_scatters_impl(::fast_io::intfpos_t off,basic_io_scatter_t<T> const *base,::std::size_t position,::std::size_t position_in_scatter) noexcept
{
	auto res{::fast_io::details::find_scatter_total_size_overflow_impl<::fast_io::uintfpos_t>(base,position)};
	constexpr
		::fast_io::intfpos_t mxv{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	if(res.position!=position)
	{
		return mxv;
	}
	return fposoffadd_nonegative(fposoffadd_nonegative(off,res.total_size),position_in_scatter);
}
}

template<typename T>
inline constexpr ::fast_io::intfpos_t fposoffadd_scatters(::fast_io::intfpos_t off,basic_io_scatter_t<T> const *base,io_scatter_status_t status) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::details::fposoffadd_scatters_impl(off,base,status.position,status.position_in_scatter);
	}
	else
	{
using io_scatter_alias_ptr

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
= io_scatter_t const*;
		return ::fast_io::details::fposoffadd_scatters_impl(off,reinterpret_cast<io_scatter_alias_ptr>(base),status.position,status.position_in_scatter);
	}
}

namespace details
{

template<::std::integral char_type>
inline constexpr ::fast_io::intfpos_t scatter_fpos_mul(::fast_io::intfpos_t ofd) noexcept
{
	constexpr
		::fast_io::intfpos_t mx{::std::numeric_limits<::fast_io::intfpos_t>::max()};
	constexpr
		::fast_io::intfpos_t ofs{mx/sizeof(char_type)};
	if(ofd>ofs)
	{
		return mx;
	}
	else
	{
		return ofd*static_cast<intfpos_t>(sizeof(char_type));
	}
}


inline constexpr ::fast_io::intfpos_t adjust_instm_offset(::std::ptrdiff_t remainspace,::fast_io::intfpos_t requested) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(remainspace>=0)]];
#endif
	constexpr auto ptrdfmn{::std::numeric_limits<::fast_io::intfpos_t>::min()};
	if(requested<ptrdfmn+remainspace)
	{
		return ptrdfmn;
	}
	return requested-remainspace;
}

}

}
