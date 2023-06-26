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
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<outstmtype>)
	{
		return scatter_write_some_bytes_overflow_define(outsm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype>)
	{
		for(::std::size_t i{};i!=n;++i)
		{
			auto [baseb,len] = pscatters[i];
			::std::byte const *base{reinterpret_cast<::std::byte const*>(baseb)};
			auto ed{base+len};
			auto written{::fast_io::details::write_some_bytes_impl(outsm,base,ed)};
			::std::size_t sz{static_cast<::std::size_t>(written-base)};
			if(sz!=len)
			{
				return {i,sz};
			}
		}
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype>)
	{
		::fast_io::details::scatter_write_all_bytes_cold_impl(outsm,pscatters,n);
		return {n,0};
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype>
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
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		auto ret{scatter_pwrite_some_bytes_cold_impl(outsm,pscatters,n,0)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,
			::fast_io::fposoffadd_scatters(0,pscatters,ret),
			::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type> const*;
		auto ret{scatter_pwrite_some_cold_impl(outsm,reinterpret_cast<scattermayalias_const_ptr>(pscatters),n,0)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,
			::fast_io::fposoffadd_scatters(0,pscatters,ret),
			::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_bytes_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::scatter_write_some_bytes_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_obuffer_basic_operations<outstmtype>)
	{
		using char_type = typename outstmtype::output_char_type;
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};

		::std::size_t buffptrdiff;	
		if constexpr(::fast_io::operations::decay::defines::has_obuffer_is_line_buffering_define<outstmtype>)
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
				using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char_type const*;
				curr=::fast_io::details::non_overlapped_copy_n(
					reinterpret_cast<char_type_const_ptr>(base),len,curr);
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
			auto ret{::fast_io::details::scatter_write_some_bytes_cold_impl(outsm,i,static_cast<::std::size_t>(e-i))};
			ret.position+=static_cast<::std::size_t>(i-pscatters);
			return ret;
		}
		return {n,0};
	}
	else
	{
		return ::fast_io::details::scatter_write_some_bytes_cold_impl(outsm,pscatters,n);
	}
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
	if constexpr(::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<outstmtype>)
	{
		scatter_write_all_bytes_overflow_define(outsm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte const *base{reinterpret_cast<::std::byte const*>(basep)};
			::fast_io::details::write_all_bytes_impl(outsm,base,base+len);
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<outstmtype>)
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
				::std::byte const *base{reinterpret_cast<::std::byte const*>(pi.base)};
				::fast_io::details::write_all_bytes_impl(outsm,base+pisc,base+pi.len);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte const *base{reinterpret_cast<::std::byte const*>(basep)};
			::fast_io::details::write_all_bytes_impl(outsm,base,base+len);
		}
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype>
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
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		scatter_pwrite_all_bytes_cold_impl(outsm,pscatters,n,0);
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,
			::fast_io::fposoffadd_scatters(0,pscatters,{n,0}),
			::fast_io::seekdir::cur);
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type> const*;
		scatter_pwrite_all_cold_impl(outsm,reinterpret_cast<scattermayalias_const_ptr>(pscatters),n,0);
		::fast_io::operations::decay::output_stream_seek_decay(outsm,
			::fast_io::fposoffadd_scatters(0,pscatters,{n,0}),
			::fast_io::seekdir::cur);
	}
}

template<typename outstmtype>
inline constexpr void scatter_write_all_bytes_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::scatter_write_all_bytes_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_obuffer_basic_operations<outstmtype>)
	{
		using char_type = typename outstmtype::output_char_type;
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};

		::std::size_t buffptrdiff;	
		if constexpr(::fast_io::operations::decay::defines::has_obuffer_is_line_buffering_define<outstmtype>)
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
				using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char_type const*;
				curr=::fast_io::details::non_overlapped_copy_n(
					reinterpret_cast<char_type_const_ptr>(base),len,curr);
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
	else
	{
		return ::fast_io::details::scatter_write_all_bytes_cold_impl(outsm,pscatters,n);
	}
}

}

}
