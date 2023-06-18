#pragma once

namespace fast_io
{

namespace operations::decay
{
template<typename instmtype>
inline constexpr io_scatter_status_t scatter_pread_until_eof_decay(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off);
template<typename instmtype>
inline constexpr void scatter_pread_until_eof_bytes_decay(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,
	::fast_io::intfpos_t off);
}

namespace details
{

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_pread_until_eof_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::details::streamreflect::has_scatter_pread_until_eof_underflow_define<instmtype>)
	{
		return scatter_pread_until_eof_underflow_define(insm,pscatters,n,off);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>)
	{
		::std::size_t const ni{n};
		for(auto psstart{pscatters};n;)
		{
			auto ret{::fast_io::details::scatter_pread_some_impl(insm,pscatters,n,off)};
			::std::size_t retpos{ret.position};
			if(retpos==n||(!retpos&&!ret.position_in_scatter))
			{
				return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+retpos),0};
			}
			off=::fast_io::fposoffadd_scatters(off,pscatters,ret);
			::std::size_t pisc{ret.position_in_scatter};
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				auto pistart{pi.base+pisc};
				auto piend{pi.base+pi.len};
				auto piit{::fast_io::operations::decay::pread_until_eof_decay(insm,pistart,piend,off)};
				if(piit!=piend)
				{
					return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+retpos),
						static_cast<::std::size_t>(piit-pi.base)};
				}
				off=::fast_io::fposoffadd_nonegative(off,pi.len-pisc);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
		return {ni,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_until_eof_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			auto ed{base+len};
			auto it{::fast_io::operations::decay::pread_until_eof_decay(insm,base,ed,off)};
			if(it!=ed)
			{
				return {static_cast<::std::size_t>(i-pscatters),
					static_cast<::std::size_t>(it-base)};
			}
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
		return {n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>)
	{
		scatter_pread_all_underflow_define(insm,pscatters,n);
		return {n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			auto ed{base+len};
			::fast_io::details::pread_all_impl(insm,base,ed,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
		return {n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_any_of_byte_pread_operations<instmtype>)
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
			return ::fast_io::details::scatter_pread_until_eof_cold_impl(insm,
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
				auto it{::fast_io::operations::decay::pread_until_eof_bytes_decay(insm,base,ed,off)};
				if(it!=ed)
				{
					return {i,static_cast<::std::size_t>(it-base)/sizeof(char_type)};
				}
				off=::fast_io::fposoffadd_nonegative(off,len);
			}
			return {n,0};
		}
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>
		&&(::fast_io::details::streamreflect::has_any_of_read_operations<instmtype>))
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,off,::fast_io::seekdir::cur);
		auto status{::fast_io::operations::decay::scatter_read_until_eof_decay(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,oldoff,::fast_io::seekdir::beg);
		return status;
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>
		&&(::fast_io::details::streamreflect::has_any_of_byte_read_operations<instmtype>))
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,off,::fast_io::seekdir::cur);
		auto status{::fast_io::operations::decay::scatter_read_until_eof_decay(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,oldoff,::fast_io::seekdir::beg);
		return status;
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_pread_until_eof_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_pread_until_eof_bytes_underflow_define<instmtype>)
	{
		return scatter_pread_until_eof_bytes_underflow_define(insm,pscatters,n,off);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>)
	{
		::std::size_t const ni{n};
		for(auto psstart{pscatters};n;)
		{
			auto ret{::fast_io::details::scatter_pread_some_bytes_impl(insm,pscatters,n,off)};
			::std::size_t retpos{ret.position};
			if(retpos==n||(!retpos&&!ret.position_in_scatter))
			{
				return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+retpos),0};
			}
			::std::size_t pisc{ret.position_in_scatter};
			off=::fast_io::fposoffadd_scatters(off,pscatters,ret);
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				auto pistart{pi.base+pisc};
				auto piend{pi.base+pi.len};
				auto piit{::fast_io::operations::decay::pread_until_eof_bytes_decay(insm,pistart,piend,off)};
				if(piit!=piend)
				{
					return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+retpos),
						piit-pi.base};
				}
				off=::fast_io::fposoffadd_nonegative(off,pi.len-pisc);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
		return {ni,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_until_eof_bytes_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			::std::byte *baseed{base+len};
			auto baseit{::fast_io::operations::decay::pread_until_eof_bytes_decay(insm,base,baseed,off)};
			if(baseit!=baseed)
			{
				return {static_cast<::std::size_t>(i-pscatters),
					static_cast<::std::size_t>(baseit-base)};
			}
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
		return {n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>)
	{
		scatter_pread_all_bytes_underflow_define(insm,pscatters,n,off);
		return {n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			::std::byte *baseed{base+len};
			::fast_io::details::pread_all_bytes_impl(insm,base,baseed,off);
			off=::fast_io::fposoffadd_nonegative(off,len);
		}
		return {n,0};
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::streamreflect::has_any_of_pread_operations<instmtype>)
	{
		using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type>*;
		return ::fast_io::details::scatter_pread_until_eof_cold_impl(
			insm,reinterpret_cast<scattermayalias_ptr>(pscatters),n,off);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&::fast_io::details::streamreflect::has_any_of_byte_read_operations<instmtype>)
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,off,::fast_io::seekdir::cur);
		auto status{::fast_io::operations::decay::scatter_read_until_eof_bytes_decay(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,oldoff,::fast_io::seekdir::beg);
		return status;
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
		::fast_io::details::streamreflect::has_any_of_read_operations<instmtype>)
	{
		auto oldoff{::fast_io::operations::decay::input_stream_seek_decay(insm,0,::fast_io::seekdir::cur)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,off,::fast_io::seekdir::cur);
		auto status{::fast_io::operations::decay::scatter_read_until_eof_bytes_decay(insm,pscatters,n)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,oldoff,::fast_io::seekdir::beg);
		return status;
	}
}

}

namespace operations::decay
{

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_pread_until_eof_decay(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::operations::decay::scatter_pread_until_eof_decay(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n,off);
	}
	else
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			off=::fast_io::details::adjust_instm_offset(ibuffer_end(insm)-ibuffer_curr(insm));
		}
		return ::fast_io::details::scatter_pread_until_eof_cold_impl(insm,pscatters,n,off);
	}
}

template<typename instmtype>
inline constexpr void scatter_pread_until_eof_bytes_decay(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n,
	::fast_io::intfpos_t off)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::operations::decay::scatter_pread_until_eof_bytes_decay(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n,off);
	}
	else
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			off=::fast_io::details::adjust_instm_offset(ibuffer_end(insm)-ibuffer_curr(insm));
		}
		return ::fast_io::details::scatter_pread_until_eof_bytes_cold_impl(insm,pscatters,n,off);
	}
}

}

}
