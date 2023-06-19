#pragma once

namespace fast_io
{

namespace details
{
template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off);

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
inline constexpr io_scatter_status_t scatter_pread_some_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_pread_some_underflow_define<instmtype>)
	{
		return scatter_pread_some_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_pread_until_eof_underflow_define<instmtype>)
	{
		return scatter_pread_until_eof_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_pread_some_underflow_define<instmtype>||
		::fast_io::operations::decay::defines::has_scatter_pread_until_eof_underflow_define<instmtype>)
	{
		for(::std::size_t i{};i!=n;++i)
		{
			auto [base,len] = pscatters[i];
			auto ed{base+len};
			char_type *it{::fast_io::details::pread_some_impl(insm,base,ed,off)};
			if(it!=ed)
			{
				return {static_cast<::std::size_t>(i-pscatters),
					static_cast<::std::size_t>(it-base)};
			}
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::operations::decay::defines::has_pread_all_underflow_define<instmtype>)
	{
		::fast_io::details::scatter_pread_all_bytes_cold_impl(insm,pscatters,n,off);
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>)
	{
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t const*;
			return ::fast_io::details::scatter_pread_some_bytes_cold_impl(insm,
				reinterpret_cast<scattermayalias_ptr>(pscatters),n,off);
		}
		else
		{
			for(::std::size_t i{};i!=n;++i)
			{
				auto [basef,len] = pscatters[i];
				auto edf{basef+len};
				::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basef))};
				::std::byte *ed{reinterpret_cast<::std::byte*>(edf)};
				auto written{::fast_io::details::pread_some_bytes_impl(insm,base,ed,off)};
				::std::size_t diff{static_cast<::std::size_t>(written-base)};
				off=::fast_io::fposoffadd_nonegative(off,diff);
				::std::size_t md{diff%sizeof(char_type)};
				::std::size_t sz{diff/sizeof(char_type)};
				if(md)
				{
					::std::size_t dfd{sizeof(char_type)-md};
					auto writtened{written+dfd};
					auto writtenit{::fast_io::details::pread_all_bytes_impl(insm,written,writtened,off)};
					if(writtenit==written)
					{
						++sz;
					}
					else
					{
						return {i,sz};
					}
					off=::fast_io::fposoffadd_nonegative(off,dfd);
				}
				if(sz!=len)
				{
					return {i,sz};
				}
			}
			return {n,0};
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_seek_define<instmtype>
		&&::fast_io::operations::decay::defines::has_any_of_read_operations<instmtype>)
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,off,::fast_io::seekdir::cur);
		auto status{::fast_io::details::scatter_read_some_cold_impl(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,oldoff,::fast_io::seekdir::beg);
		return status;
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_seek_bytes_define<instmtype>
		&&(::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>))
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,off,::fast_io::seekdir::cur);
		auto status{::fast_io::details::scatter_read_some_cold_impl(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,oldoff,::fast_io::seekdir::beg);
		return status;
	}
}

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_pread_some_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::scatter_pread_some_impl(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n);
	}
	else
	{
		if constexpr(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			off=::fast_io::details::adjust_instm_offset(ibuffer_end(insm)-ibuffer_curr(insm));
		}
		return ::fast_io::details::scatter_pread_some_cold_impl(insm,pscatters,n,off);
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_pread_all_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::operations::decay::defines::has_scatter_pread_all_underflow_define<instmtype>)
	{
		scatter_pread_all_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_pread_all_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::pread_all_impl(insm,base,base+len,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_pread_some_underflow_define<instmtype>)
	{
		for(;;)
		{
			auto ret{::fast_io::details::scatter_pread_some_impl(insm,pscatters,n,off)};
			::std::size_t retpos{ret.position};
			if(retpos==n)
			{
				return;
			}
			if(!retpos)
			{
				::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file);
			}
			off=::fast_io::fposoffadd_scatters(off,pscatters,ret);
			::std::size_t pisc{ret.position_in_scatter};
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::fast_io::details::pread_all_impl(insm,pi.base+pisc,pi.base+pi.len,off);
				off=::fast_io::fposoffadd_nonegative(off,pi.len-pisc);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_pread_some_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::pread_all_impl(insm,base,base+len,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>)
	{
		using char_type = typename instmtype::input_char_type;
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t*;
			::fast_io::details::scatter_pread_all_bytes_cold_impl(insm,
				reinterpret_cast<scattermayalias_ptr>(pscatters),n,off);
		}
		else
		{
			for(::std::size_t i{};i!=n;++i)
			{
				auto [basef,len] = pscatters[i];
				auto edf{basef+len};
				::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basef))};
				::std::byte *ed{reinterpret_cast<::std::byte*>(edf)};
				::fast_io::details::pread_all_bytes_impl(insm,base,ed,off);
				off=::fast_io::fposoffadd_nonegative(off,len);
			}
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_seek_define<instmtype>
		&&::fast_io::operations::decay::defines::has_any_of_read_operations<instmtype>)
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,off,::fast_io::seekdir::cur);
		::fast_io::details::scatter_read_all_cold_impl(insm,pscatters,n);
		::fast_io::operations::decay::input_stream_seek_decay(insm,oldoff,::fast_io::seekdir::beg);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_seek_bytes_define<instmtype>
		&&(::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>))
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,off,::fast_io::seekdir::cur);
		::fast_io::details::scatter_read_all_cold_impl(insm,pscatters,n);
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,oldoff,::fast_io::seekdir::beg);
	}
}

template<typename instmtype>
inline constexpr void scatter_pread_all_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::scatter_pread_all_impl(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n);
	}
	else
	{
		if constexpr(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			off=::fast_io::details::adjust_instm_offset(ibuffer_end(insm)-ibuffer_curr(insm));
		}
		return ::fast_io::details::scatter_pread_all_cold_impl(insm,pscatters,n,off);
	}
}

}

}
