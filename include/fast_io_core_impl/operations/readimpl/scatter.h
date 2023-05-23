#pragma once

namespace fast_io
{

namespace details
{
template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_read_some_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_read_all_bytes_cold_impl(instmtype insm,
	io_scatter_t const *pscatters,
	::std::size_t n);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_read_all_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr io_scatter_status_t scatter_read_some_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<instmtype>)
	{
		return scatter_read_some_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		for(::std::size_t i{};i!=n;++i)
		{
			auto [base,len] = pscatters[i];
			auto ed{base+len};
			auto written{::fast_io::details::read_some_impl(insm,base,ed)};
			::std::size_t sz{static_cast<::std::size_t>(written-base)};
			if(sz!=len)
			{
				return {i,sz};
			}
		}
		return {n,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>
		||::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
	{
		scatter_read_all_cold_impl(insm,pscatters,n);
		return {n,0};
	}
	else if constexpr((::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>))
	{
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t*;
			return ::fast_io::details::scatter_read_some_bytes_cold_impl(insm,
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
				auto written{::fast_io::details::read_some_bytes_impl(insm,base,ed)};
				::std::size_t sz{static_cast<::std::size_t>(written-base)};
				::std::size_t md{sz%sizeof(char_type)};
				if(md)
				{
					::std::size_t dfd{sizeof(char_type)-md};
					::fast_io::details::read_all_bytes_impl(insm,written,written+dfd);
					written+=dfd;
					sz+=dfd;
				}
				if(sz!=len)
				{
					return {i,sz};
				}
			}
			return {n,0};
		}
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>
	))
	{
		auto ret{scatter_pread_some_cold_impl(insm,pscatters,n,0)};
		::fast_io::details::input_stream_seek_impl(insm,fposoffadd_scatters(0,pscatters,ret),::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>
	))
	{
		auto ret{scatter_pread_some_cold_impl(insm,pscatters,n,0)};
		::fast_io::details::input_stream_seek_bytes_impl(insm,
			::fast_io::details::scatter_fpos_mul<char_type>(::fast_io::fposoffadd_scatters(0,pscatters,ret)),
			::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename instmtype>
inline constexpr io_scatter_status_t scatter_read_some_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(insm)};
		return ::fast_io::details::scatter_read_some_impl(::fast_io::details::input_stream_unlocked_ref_impl(insm),pscatters,n);
	}
	else if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
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
			auto ret{::fast_io::details::scatter_read_some_cold_impl(insm,i,static_cast<::std::size_t>(e-i))};
			ret.position+=static_cast<::std::size_t>(i-pscatters);
			return ret;
		}
		return {n,0};
	}
	else
	{
		return scatter_read_some_cold_impl(insm,pscatters,n);
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void scatter_read_all_cold_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>)
	{
		scatter_read_all_underflow_define(insm,pscatters,n);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::read_all_impl(insm,base,base+len);
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<instmtype>)
	{
		for(;;)
		{
			auto ret{::fast_io::details::scatter_read_some_impl(insm,pscatters,n)};
			::std::size_t retpos{ret.position};
			if(retpos==n)
			{
				return;
			}
			::std::size_t pisc{ret.position_in_scatter};
			if(pisc)
			{
				auto pi = pscatters[ret.position];
				::fast_io::details::read_all_impl(insm,pi.base+pisc,pi.len);
				++retpos;
			}
			pscatters+=retpos;
			n-=retpos;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		for(auto i{pscatters},e{pscatters+n};i!=e;++i)
		{
			auto [base,len] = *i;
			::fast_io::details::read_all_impl(insm,base,base+len);
		}
	}
	else if constexpr((::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>))
	{
		if constexpr(sizeof(char_type)==1)
		{
			using scattermayalias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			=
			io_scatter_t*;
			::fast_io::details::scatter_read_all_bytes_cold_impl(insm,
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
				::fast_io::details::read_all_bytes_impl(insm,base,ed);
			}
		}
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>
	))
	{
		scatter_pread_all_cold_impl(insm,pscatters,n,0);
		::fast_io::details::input_stream_seek_impl(insm,::fast_io::fposoffadd_scatters(0,pscatters,{n,0}),::fast_io::seekdir::cur);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>
	))
	{
		scatter_pread_all_cold_impl(insm,pscatters,n,0);
		::fast_io::details::input_stream_seek_bytes_impl(insm,
			::fast_io::details::scatter_fpos_mul<char_type>(::fast_io::fposoffadd_scatters(0,pscatters,{n,0})),
			::fast_io::seekdir::cur);
	}
}

template<typename instmtype>
inline constexpr void scatter_read_all_impl(instmtype insm,
	basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
	::std::size_t n)
{
	if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(insm)};
		return ::fast_io::details::scatter_read_all_impl(::fast_io::details::input_stream_unlocked_ref_impl(insm),pscatters,n);
	}
	else if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
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
			return ::fast_io::details::scatter_read_all_cold_impl(insm,i,static_cast<::std::size_t>(e-i));
		}
	}
	else
	{
		return ::fast_io::details::scatter_read_all_cold_impl(insm,pscatters,n);
	}
}

}

}

