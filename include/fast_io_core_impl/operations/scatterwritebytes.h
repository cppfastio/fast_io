namespace fast_io
{

namespace details
{

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_write_some_bytes_cold_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<outstmtype>)
	{
		return scatter_write_some_bytes_overflow_define(outsm,pscatters,n);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<outstmtype>)
	{
		::std::size_t total_len{};
		for(::std::size_t i{};i!=n;++i)
		{
			auto [base,len] = pscatters[i];
			auto ed{base+len};
			auto written{::fast_io::details::write_some_bytes_impl(outsm,base,ed)};
			::std::size_t sz{static_cast<::std::size_t>(written-base)};
			total_len+=sz;
			if(sz!=len)
			{
				return {total_len,i,sz};
			}
		}
		return {total_len,n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<outstmtype>)
	{
		scatter_write_all_bytes_overflow_define(outsm,pscatters,n);
		return {::fast_io::scatter_total_size(pscatters,n),n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<outstmtype>)
	{
		::std::size_t total_len{};
		for(::std::size_t i{};i!=n;++i)
		{
			auto [base,len] = pscatters[i];
			::fast_io::details::write_all_bytes_impl(outsm,base,base+len);
			total_len+=len;
		}
		return {total_len,n,0};
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::details::streamreflect::has_scatter_write_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_some_overflow_define<outstmtype>
	))
	{
		using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type> const*;
		return ::fast_io::details::scatter_write_some_cold_impl(
			outsm,reinterpret_cast<scattermayalias_const_ptr>(pscatters),n);
	}
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_bytes_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	using char_type = typename outstmtype::output_char_type;
	char_type *curr{obuffer_curr(outsm)};
	char_type *ed{obuffer_end(outsm)};

	::std::size_t buffptrdiff;	
	if constexpr(!::fast_io::noline_buffer_output_stream<outstmtype>)
	{
		::std::ptrdiff_t pptrdf{ed-curr};
		if(pptrdf<0)
		{
			pptrdf=0;
		}
		buffptrdiff=static_cast<::std::size_t>(pptrdf);
	}
	else
	{
		buffptrdiff=static_cast<::std::size_t>(ed-curr);
	}

	::std::size_t total_len{};
	auto i{pscatters},e{pscatters+n};
	for(;i!=e;++i)
	{
		auto [base,len] = *i;
		if(len<buffptrdiff)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
			curr=::fast_io::details::non_overlapped_copy_n(base,len,curr);
			buffptrdiff-=len;
			total_len+=len;
		}
		else
		{
			break;
		}
	}
	obuffer_set_curr(outsm,curr);
	if(i!=e)
#if __has_cpp_attribute(unlikely)
[[unlikely]]
#endif
	{
		auto ret{::fast_io::details::scatter_write_some_bytes_cold_impl(outsm,i,static_cast<::std::size_t>(e-i))};
		ret.total_len=::fast_io::details::intrinsics::add_or_overflow_die(total_len,ret.total_len);
		ret.position+=static_cast<::std::size_t>(i-pscatters);
		return ret;
	}
	return {total_len,n,0};
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_write_all_bytes_cold_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<outstmtype>)
	{
		scatter_write_all_bytes_overflow_define(outsm,pscatters,n);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::write_all_bytes_impl(outsm,base,base+len);
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<outstmtype>)
	{
		for(;;)
		{
			auto ret{::fast_io::details::scatter_write_some_bytes_impl(outsm,pscatters,n)};
			::std::size_t retpos{ret.position};
			if(retpos==n)
			{
				return;
			}
			::std::size_t pisc{ret.position_in_scatter};
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::fast_io::details::write_all_bytes_impl(outsm,pi.base+pisc,pi.len);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::write_all_bytes_impl(outsm,base,base+len);
		}
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::details::streamreflect::has_scatter_write_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_some_overflow_define<outstmtype>
	))
	{
		using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type> const*;
		::fast_io::details::scatter_write_all_impl(
			outsm,reinterpret_cast<scattermayalias_const_ptr>(pscatters),n);
	}
}

template<typename outstmtype>
inline constexpr void scatter_write_all_bytes_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	using char_type = typename outstmtype::output_char_type;
	char_type *curr{obuffer_curr(outsm)};
	char_type *ed{obuffer_end(outsm)};

	::std::size_t buffptrdiff;	
	if constexpr(!::fast_io::noline_buffer_output_stream<outstmtype>)
	{
		::std::ptrdiff_t pptrdf{ed-curr};
		if(pptrdf<0)
		{
			pptrdf=0;
		}
		buffptrdiff=static_cast<::std::size_t>(pptrdf);
	}
	else
	{
		buffptrdiff=static_cast<::std::size_t>(ed-curr);
	}

	auto i{pscatters},e{pscatters+n};
	for(;i!=e;++i)
	{
		auto [base,len] = *i;
		if(len<buffptrdiff)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
			curr=::fast_io::details::non_overlapped_copy_n(base,len,curr);
			buffptrdiff-=len;
		}
		else
		{
			break;
		}
	}
	obuffer_set_curr(outsm,curr);
	if(i!=e)
#if __has_cpp_attribute(unlikely)
	[[unlikely]]
#endif
	{
		return ::fast_io::details::scatter_write_all_bytes_cold_impl(outsm,i,static_cast<::std::size_t>(e-i));
	}
}

}

}
