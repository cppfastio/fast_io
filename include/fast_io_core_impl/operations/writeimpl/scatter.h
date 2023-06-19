#pragma once

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
	::std::size_t n);

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_write_all_bytes_cold_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n);

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_write_all_cold_impl(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
	::std::size_t n);

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_write_some_cold_impl(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
	::std::size_t n)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>)
	{
		return scatter_write_some_overflow_define(outsm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype>)
	{
		for(::std::size_t i{};i!=n;++i)
		{
			auto [base,len] = pscatters[i];
			auto ed{base+len};
			auto written{::fast_io::details::write_some_impl(outsm,base,ed)};
			::std::size_t sz{static_cast<::std::size_t>(written-base)};
			if(sz!=len)
			{
				return {i,sz};
			}
		}
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype>
		||::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype>)
	{
		scatter_write_all_cold_impl(outsm,pscatters,n);
		return {n,0};
	}
	else if constexpr((::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<outstmtype>))
	{
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t const*;
			return ::fast_io::details::scatter_write_some_bytes_cold_impl(outsm,
				reinterpret_cast<scattermayalias_const_ptr>(pscatters),n);
		}
		else
		{
			for(::std::size_t i{};i!=n;++i)
			{
				auto [basef,len] = pscatters[i];
				auto edf{basef+len};
				::std::byte const *base{reinterpret_cast<::std::byte const*>(basef)};
				::std::byte const *ed{reinterpret_cast<::std::byte const*>(edf)};
				auto written{::fast_io::details::write_some_bytes_impl(outsm,base,ed)};
				::std::size_t diff{static_cast<::std::size_t>(written-base)};
				::std::size_t md{diff%sizeof(char_type)};
				::std::size_t sz{diff/sizeof(char_type)};
				if(md)
				{
					::std::size_t dfd{sizeof(char_type)-md};
					::fast_io::details::write_all_bytes_impl(outsm,written,written+dfd);
					++sz;
				}
				if(sz!=len)
				{
					return {i,sz};
				}
			}
			return {n,0};
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		auto ret{scatter_pwrite_some_cold_impl(outsm,pscatters,n,0)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,fposoffadd_scatters(0,pscatters,ret),::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		auto ret{scatter_pwrite_some_cold_impl(outsm,pscatters,n,0)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,
			::fast_io::details::scatter_fpos_mul<char_type>(::fast_io::fposoffadd_scatters(0,pscatters,ret)),
			::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_impl(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::scatter_write_some_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),pscatters,n);
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
			auto ret{::fast_io::details::scatter_write_some_cold_impl(outsm,i,static_cast<::std::size_t>(e-i))};
			ret.position+=static_cast<::std::size_t>(i-pscatters);
			return ret;
		}
		return {n,0};
	}
	else
	{
		return scatter_write_some_cold_impl(outsm,pscatters,n);
	}
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_write_all_cold_impl(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
	::std::size_t n)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype>)
	{
		scatter_write_all_overflow_define(outsm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::write_all_impl(outsm,base,base+len);
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>)
	{
		for(;;)
		{
			auto ret{::fast_io::details::scatter_write_some_impl(outsm,pscatters,n)};
			::std::size_t retpos{ret.position};
			if(retpos==n)
			{
				return;
			}
			::std::size_t pisc{ret.position_in_scatter};
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::fast_io::details::write_all_impl(outsm,pi.base+pisc,pi.base+pi.len);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::write_all_impl(outsm,base,base+len);
		}
	}
	else if constexpr((::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<outstmtype>))
	{
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t const*;
			::fast_io::details::scatter_write_all_bytes_cold_impl(outsm,
				reinterpret_cast<scattermayalias_const_ptr>(pscatters),n);
		}
		else
		{
			for(::std::size_t i{};i!=n;++i)
			{
				auto [basef,len] = pscatters[i];
				auto edf{basef+len};
				::std::byte const *base{reinterpret_cast<::std::byte const*>(basef)};
				::std::byte const *ed{reinterpret_cast<::std::byte const*>(edf)};
				::fast_io::details::write_all_bytes_impl(outsm,base,ed);
			}
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		scatter_pwrite_all_cold_impl(outsm,pscatters,n,0);
		::fast_io::operations::decay::output_stream_seek_decay(outsm,::fast_io::fposoffadd_scatters(0,pscatters,{n,0}),::fast_io::seekdir::cur);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		scatter_pwrite_all_cold_impl(outsm,pscatters,n,0);
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,
			::fast_io::details::scatter_fpos_mul<char_type>(::fast_io::fposoffadd_scatters(0,pscatters,{n,0})),
			::fast_io::seekdir::cur);
	}
}

template<typename outstmtype>
inline constexpr void scatter_write_all_impl(outstmtype outsm,
	basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::scatter_write_all_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),pscatters,n);
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
			return ::fast_io::details::scatter_write_all_cold_impl(outsm,i,static_cast<::std::size_t>(e-i));
		}
	}
	else
	{
		return ::fast_io::details::scatter_write_all_cold_impl(outsm,pscatters,n);
	}
}

}

}

