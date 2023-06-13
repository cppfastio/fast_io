#pragma once

namespace fast_io
{

namespace operations
{
template<typename instmtype>
inline constexpr io_scatter_status_t scatter_read_until_eof_decay(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n);

template<typename instmtype>
inline constexpr void scatter_read_until_eof_bytes_decay(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n);

}

namespace details
{
template<typename instmtype>
inline constexpr io_scatter_status_t scatter_read_until_eof_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n);

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_pread_until_eof_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n,::fast_io::intfpos_t off);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_read_until_eof_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_read_until_eof_bytes_underflow_define<instmtype>)
	{
		return scatter_read_until_eof_bytes_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		::std::size_t const ni{n};
		for(auto psstart{pscatters};n;)
		{
			auto ret{scatter_read_some_bytes_underflow_define(insm,pscatters,n)};
			::std::size_t retpos{ret.position};
			if(retpos==n||(!retpos&&!ret.position_in_scatter))
			{
				return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+retpos),0};
			}
			::std::size_t pisc{ret.position_in_scatter};
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(pi.base))};
				auto pistart{base+pisc};
				auto piend{base+pi.len};
				auto piit{::fast_io::operations::decay::read_until_eof_bytes_decay(insm,pistart,piend)};
				if(piit!=piend)
				{
					return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+retpos),
						static_cast<::std::size_t>(piit-base)};
				}
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
		return {ni,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_read_some_bytes_underflow_define<instmtype>||
			::fast_io::operations::decay::defines::has_read_until_eof_bytes_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			auto baseed{base+len};
			::std::byte *baseit{::fast_io::operations::decay::read_until_eof_bytes_decay(insm,base,baseed)};
			if(baseit!=baseed)
			{
				return {static_cast<::std::size_t>(i-pscatters),
					static_cast<::std::size_t>(baseit-base)};
			}
		}
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_read_all_bytes_underflow_define<instmtype>)
	{
		scatter_read_all_bytes_underflow_define(insm,pscatters,n);
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_read_all_bytes_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			::fast_io::details::read_all_bytes_impl(insm,base,base+len);
		}
		return {n,0};
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>)
	{
		using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type>*;
		return ::fast_io::details::scatter_read_until_eof_cold_impl(
			insm,reinterpret_cast<scattermayalias_ptr>(pscatters),n);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
		::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>)
	{
		auto status{::fast_io::details::scatter_pread_until_eof_cold_impl(insm,pscatters,n,0)};
		::fast_io::details::input_stream_seek_bytes_impl(insm,
			::fast_io::fposoffadd_scatters(0,pscatters,status),
			::fast_io::seekdir::cur);
		return status;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
		::fast_io::operations::decay::defines::has_any_of_pread_operations<instmtype>)
	{
		using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		basic_io_scatter_t<char_type>*;
		auto status{scatter_pread_until_eof_cold_impl(insm,reinterpret_cast<scattermayalias_ptr>(pscatters),n,0)};
		::fast_io::details::input_stream_seek_impl(insm,
			::fast_io::fposoffadd_scatters(0,pscatters,status),
			::fast_io::seekdir::cur);
		return status;
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_read_until_eof_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::operations::decay::defines::has_scatter_read_until_eof_underflow_define<instmtype>)
	{
		return scatter_read_until_eof_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_read_some_underflow_define<instmtype>)
	{
		::std::size_t const ni{n};
		for(auto psstart{pscatters};n;)
		{
			auto [position,position_in_scatter]{::fast_io::details::scatter_read_some_impl(insm,pscatters,n)};
			if(position==n||(!position&&!position_in_scatter))
			{
				return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+position),0};
			}
			if(position_in_scatter)
			{
				auto [base2,len] = pscatters[position];
				auto base{const_cast<char_type*>(base2)};
				auto pistart{base+position_in_scatter};
				auto piend{base+len};
				auto piit{::fast_io::operations::decay::read_until_eof_decay(insm,pistart,piend)};
				if(piit!=piend)
				{
					return {static_cast<::std::size_t>(static_cast<::std::size_t>(pscatters-psstart)+position),
						static_cast<::std::size_t>(piit-base)};
				}
				++position;
			}
			pscatters+=position;
			n-=position;
		}
		return {ni,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_read_some_underflow_define<instmtype>||
		::fast_io::operations::decay::defines::has_read_until_eof_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [basep,len] = *i;
			::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basep))};
			::std::byte *baseed{base+len};
			auto baseit{::fast_io::operations::decay::read_until_eof_decay(insm,base,baseed)};
			if(baseit!=baseed)
			{
				return {static_cast<::std::size_t>(i-pscatters),
					static_cast<::std::size_t>(baseit-base)};
			}
		}
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_scatter_read_all_underflow_define<instmtype>)
	{
		scatter_read_all_underflow_define(insm,pscatters,n);
		return {n,0};
	}
	else if constexpr(::fast_io::operations::decay::defines::has_read_all_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::read_all_impl(insm,base,base+len);
		}
		return {n,0};
	}
	else if constexpr((::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>))
	{
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t*;
			return ::fast_io::details::scatter_read_until_eof_bytes_cold_impl(insm,
				reinterpret_cast<scattermayalias_ptr>(pscatters),n);
		}
		else
		{
			for(::std::size_t i{};i!=n;++i)
			{
				auto [basef,len] = pscatters[i];
				auto edf{basef+len};
				::std::byte *base{reinterpret_cast<::std::byte*>(const_cast<void*>(basef))};
				::std::byte *ed{reinterpret_cast<::std::byte*>(edf)};
				::std::byte *baseit{::fast_io::operations::decay::read_until_eof_bytes_decay(insm,base,ed)};
				if(baseit!=ed)
				{
					return {i,static_cast<::std::size_t>(baseit-base)/sizeof(char_type)};
				}
			}
		}
		return {n,0};
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(::fast_io::operations::decay::defines::has_any_of_pread_operations<instmtype>))
	{
		auto status{::fast_io::details::scatter_pread_until_eof_cold_impl(insm,pscatters,n,0)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,::fast_io::fposoffadd_scatters(0,pscatters,status),::fast_io::seekdir::cur);
		return status;
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>))
	{
		auto status{::fast_io::details::scatter_pread_until_eof_cold_impl(insm,pscatters,n,0)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,
			::fast_io::details::scatter_fpos_mul<char_type>(::fast_io::fposoffadd_scatters(0,pscatters,status)),
			::fast_io::seekdir::cur);
		return status;
	}
}

}


namespace operations::decay
{

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_read_until_eof_decay(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::operations::decay::scatter_read_until_eof_decay(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
	{
		using char_type = typename instmtype::input_char_type;
		char_type *curr{ibuffer_curr(insm)};
		char_type *ed{ibuffer_end(insm)};

		::std::size_t buffptrdiff{static_cast<::std::size_t>(ed-curr)};

		auto i{pscatters},e{pscatters+n};
		for(;i!=e;++i)
		{
			auto [base,len] = *i;
			if(len<buffptrdiff)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
			{
				::fast_io::details::non_overlapped_copy_n(curr,len,base);
				curr+=len;
				buffptrdiff-=len;
			}
			else
			{
				break;
			}
		}
		ibuffer_set_curr(insm,curr);
		if(i!=e)
#if __has_cpp_attribute(unlikely)
	[[unlikely]]
#endif
		{
			auto ret{::fast_io::details::scatter_read_until_eof_cold_impl(insm,i,static_cast<::std::size_t>(e-i))};
			ret.position+=static_cast<::std::size_t>(i-pscatters);
			return ret;
		}
		return {n,0};
	}
	else
	{
		return ::fast_io::details::scatter_read_until_eof_cold_impl(insm,pscatters,n);
	}
}

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_read_until_eof_bytes_decay(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::operations::decay::scatter_read_until_eof_bytes_decay(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),pscatters,n);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
	{
		using char_type = typename instmtype::input_char_type;
		char_type *curr{ibuffer_curr(insm)};
		char_type *ed{ibuffer_end(insm)};

		::std::size_t buffptrdiff{static_cast<::std::size_t>(ed-curr)};
		auto i{pscatters},e{pscatters+n};
		for(;i!=e;++i)
		{
			auto [base,len] = *i;
			if(len<buffptrdiff)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
			{
				using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= char_type*;
				::fast_io::details::non_overlapped_copy_n(curr,len,
					reinterpret_cast<char_type_ptr>(base));
				curr+=len;
				buffptrdiff-=len;
			}
			else
			{
				break;
			}
		}
		ibuffer_set_curr(insm,curr);
		if(i!=e)
#if __has_cpp_attribute(unlikely)
		[[unlikely]]
#endif
		{
			auto ret{::fast_io::details::scatter_read_until_eof_bytes_cold_impl(insm,i,static_cast<::std::size_t>(e-i))};
			ret.position+=static_cast<::std::size_t>(i-pscatters);
			return ret;
		}
		return {n,0};
	}
	else
	{
		return ::fast_io::details::scatter_read_until_eof_bytes_cold_impl(insm,pscatters,n);
	}
}

}

}
