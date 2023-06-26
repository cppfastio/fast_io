namespace fast_io
{

namespace details
{

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_pwrite_all_bytes_cold_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off);

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes_cold_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n,intfpos_t off)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		return scatter_pwrite_some_bytes_overflow_define(outsm,pscatters,n,off);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		for(::std::size_t i{};i!=n;++i)
		{
			auto [baseb,len] = pscatters[i];
			::std::byte const *base{reinterpret_cast<::std::byte const*>(baseb)};
			auto ed{base+len};
			auto written{::fast_io::details::pwrite_some_bytes_impl(outsm,base,ed,off)};
			::std::ptrdiff_t dfsz{written-base};
			::std::size_t sz{static_cast<::std::size_t>(written-base)};
			if(sz!=len)
			{
				return {i,sz};
			}
			off=::fast_io::fposoffadd_nonegative(off,dfsz);
		}
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>
		||::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		::fast_io::details::scatter_pwrite_all_bytes_cold_impl(outsm,pscatters,n,off);
		return {n,0};
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>
	))
	{
		using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type> const*;
		return ::fast_io::details::scatter_pwrite_some_cold_impl(
			outsm,reinterpret_cast<scattermayalias_const_ptr>(pscatters),n,off);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<outstmtype>
	))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,off,::fast_io::seekdir::cur);
		auto ret{::fast_io::details::scatter_write_some_bytes_cold_impl(outsm,pscatters,n)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,oldoff,::fast_io::seekdir::beg);
		return ret;
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>
	))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_decay(outsm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,off,::fast_io::seekdir::cur);
		auto ret{::fast_io::details::scatter_write_some_bytes_cold_impl(outsm,pscatters,n)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,oldoff,::fast_io::seekdir::beg);
		return ret;
	}
}

template<typename outstmtype>
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::scatter_pwrite_some_bytes_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),pscatters,n,off);
	}
	else
	{
		if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<outstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(outsm);
		}
		return ::fast_io::details::scatter_pwrite_some_bytes_cold_impl(outsm,pscatters,n,off);
	}
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_pwrite_all_bytes_cold_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		scatter_pwrite_all_bytes_overflow_define(outsm,pscatters,n,off);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte const *base{reinterpret_cast<::std::byte const*>(basep)};
			::fast_io::details::pwrite_all_bytes_impl(outsm,base,base+len,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		for(;;)
		{
			auto ret{::fast_io::details::scatter_pwrite_some_bytes_impl(outsm,pscatters,n,off)};
			::std::size_t retpos{ret.position};
			if(retpos==n)
			{
				return;
			}
			::std::size_t pisc{ret.position_in_scatter};
			off=::fast_io::fposoffadd_scatters(off,pscatters,ret);
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::fast_io::details::pwrite_all_bytes_impl(outsm,pi.base+pisc,pi.base+pi.len,off);
				off=::fast_io::fposoffadd_nonegative(off,pi.len-pisc);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte const *base{reinterpret_cast<::std::byte const*>(basep)};
			::fast_io::details::pwrite_all_bytes_impl(outsm,base,base+len,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>
	))
	{
		using scattermayalias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type> const*;
		::fast_io::details::scatter_pwrite_all_impl(
			outsm,reinterpret_cast<scattermayalias_const_ptr>(pscatters),n,off);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<outstmtype>
	))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,off,::fast_io::seekdir::cur);
		::fast_io::details::scatter_write_all_bytes_impl(outsm,pscatters,n);
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,oldoff,::fast_io::seekdir::beg);
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype>||
		::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>
	))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_decay(outsm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,off,::fast_io::seekdir::cur);
		::fast_io::details::scatter_write_all_bytes_impl(outsm,pscatters,n);
		::fast_io::operations::decay::output_stream_seek_decay(outsm,oldoff,::fast_io::seekdir::beg);
	}
}

template<typename outstmtype>
inline constexpr void scatter_pwrite_all_bytes_impl(outstmtype outsm,
	io_scatter_t const *pscatters,
	::std::size_t n,
	::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::scatter_pwrite_all_bytes_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),pscatters,n,off);
	}
	else
	{
		if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<outstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(outsm);
		}
		return ::fast_io::details::scatter_pwrite_all_bytes_cold_impl(outsm,pscatters,n,off);
	}
}

}

}
