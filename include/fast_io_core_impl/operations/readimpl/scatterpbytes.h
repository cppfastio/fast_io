namespace fast_io
{

namespace details
{

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_pread_all_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,intfpos_t off)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>)
	{
		return scatter_pread_some_bytes_underflow_define(insm,pscatters,n,off);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_until_eof_bytes_underflow_define<instmtype>)
	{
		return scatter_pread_until_eof_bytes_underflow_define(insm,pscatters,n,off);
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
			::fast_io::details::streamreflect::has_pread_until_eof_underflow_define<instmtype>)
	{
		for(::std::size_t i{};i!=n;++i)
		{
			auto [baseb,len] = pscatters[i];
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(baseb))};
			auto ed{base+len};
			auto written{::fast_io::details::pread_some_bytes_impl(insm,base,ed,off)};
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
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>
		||::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>)
	{
		::fast_io::details::scatter_pread_all_bytes_cold_impl(insm,pscatters,n,off);
		return {n,0};
	}
	else if constexpr(sizeof(char_type)==1&&
	(
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>
	))
	{
		using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type>*;
		return ::fast_io::details::scatter_pread_some_cold_impl(
			insm,reinterpret_cast<scattermayalias_ptr>(pscatters),n,off);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_any_of_byte_read_operations<instmtype>
	))
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,off,::fast_io::seekdir::cur);
		auto ret{::fast_io::details::scatter_read_some_bytes_cold_impl(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,oldoff,::fast_io::seekdir::beg);
		return ret;
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
		::fast_io::details::streamreflect::has_any_of_read_operations<instmtype>)
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,off,::fast_io::seekdir::cur);
		auto ret{::fast_io::details::scatter_read_some_bytes_cold_impl(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,oldoff,::fast_io::seekdir::beg);
		return ret;
	}
}

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_pread_some_bytes_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::scatter_pread_some_bytes_impl(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n,off);
	}
	else
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			off=::fast_io::details::adjust_instm_offset(ibuffer_end(insm)-ibuffer_curr(insm));
		}
		return ::fast_io::details::scatter_pread_some_bytes_cold_impl(insm,pscatters,n,off);
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_pread_all_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>)
	{
		scatter_pread_all_bytes_underflow_define(insm,pscatters,n,off);
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			::fast_io::details::pread_all_bytes_impl(insm,base,base+len,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>)
	{
		for(;;)
		{
			auto ret{::fast_io::details::scatter_pread_some_bytes_impl(insm,pscatters,n,off)};
			::std::size_t retpos{ret.position};
			if(retpos==n)
			{
				return;
			}
			if(!retpos)
			{
				::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file);
			}
			::std::size_t pisc{ret.position_in_scatter};
			off=::fast_io::fposoffadd_scatters(off,pscatters,ret);
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::fast_io::details::pread_all_bytes_impl(insm,pi.base+pisc,pi.base+pi.len,off);
				off=::fast_io::fposoffadd_nonegative(off,pi.len-pisc);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			::fast_io::details::pread_all_bytes_impl(insm,base,base+len,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::streamreflect::has_any_of_read_operations<instmtype>)
	{
		using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type>*;
		::fast_io::details::scatter_pread_all_impl(
			insm,reinterpret_cast<scattermayalias_ptr>(pscatters),n,off);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_any_of_byte_read_operations<instmtype>
	))
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,off,::fast_io::seekdir::cur);
		::fast_io::details::scatter_read_all_bytes_impl(insm,pscatters,n);
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,oldoff,::fast_io::seekdir::beg);
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
		::fast_io::details::streamreflect::has_any_of_read_operations<instmtype>)
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,off,::fast_io::seekdir::cur);
		::fast_io::details::scatter_read_all_bytes_impl(insm,pscatters,n);
		::fast_io::operations::decay::input_stream_seek_decay(insm,oldoff,::fast_io::seekdir::beg);
	}
}

template<typename instmtype>
inline constexpr void scatter_pread_all_bytes_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,
	::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::scatter_pread_all_bytes_impl(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n,off);
	}
	else
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			off=::fast_io::details::adjust_instm_offset(ibuffer_end(insm)-ibuffer_curr(insm));
		}
		return ::fast_io::details::scatter_pread_all_bytes_cold_impl(insm,pscatters,n,off);
	}
}

}

}
