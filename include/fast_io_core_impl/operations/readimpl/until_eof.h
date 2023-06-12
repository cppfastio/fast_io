#pragma once

namespace fast_io
{

namespace details
{

template<typename instmtype>
inline constexpr typename instmtype::input_char_type* pread_until_eof_cold_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last,::fast_io::intfpos_t off);

template<typename instmtype>
inline constexpr void pread_until_eof_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last,::fast_io::intfpos_t off);

template<typename instmtype>
inline constexpr ::std::byte* read_until_eof_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr typename instmtype::input_char_type* read_until_eof_cold_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_until_eof_underflow_define<instmtype>)
	{
		return read_until_eof_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_until_eof_underflow_define<instmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{first,len};
		auto [position,position_in_scatter]{scatter_read_until_eof_underflow_define(insm,__builtin_addressof(sc),1)};
		if(position)
		{
			return last;
		}
		return first+position_in_scatter;
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			while(first!=last)
			{
				auto it{read_some_underflow_define(insm,first,last)};
				if(first==it)
				{
					return it;
				}
				first=it;
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					auto res{non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),first)};
					ibuffer_set_curr(insm,curr+itdiff);
					return res;
				}
			}
			return last;
		}
		else
		{
			while(first!=last)
			{
				auto it{read_some_underflow_define(insm,first,last)};
				if(first==it)
				{
					return it;
				}
				first=it;
			}
			return last;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				basic_io_scatter_t<char_type> sc{first,len};
				auto [position,position_io_scatter]{scatter_read_some_underflow_define(insm,__builtin_addressof(sc),1)};
				if(position==1)
				{
					return last;
				}
				if(position_io_scatter==0)
				{
					return first;
				}
				first+=position_io_scatter;
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					first=non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),first);
					ibuffer_set_curr(insm,curr+itdiff);
					return first;
				}
			}
			return last;
		}
		else
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				basic_io_scatter_t<char_type> sc{first,len};
				auto [position,position_io_scatter]{scatter_read_some_underflow_define(insm,__builtin_addressof(sc),1)};
				if(position==1)
				{
					return last;
				}
				if(position_io_scatter==0)
				{
					return first;
				}
				first+=position_io_scatter;
			}
			return last;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
	{
		read_all_underflow_define(insm,first,last);
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>)
	{
		basic_io_scatter_t<char_type> sc{first,static_cast<::std::size_t>(last-first)};
		scatter_read_all_underflow_define(insm,__builtin_addressof(sc),1);
		return last;
	}
	else if constexpr(
		(::fast_io::details::streamreflect::has_any_of_byte_read_operations<instmtype>))
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		return reinterpret_cast<char_type_ptr>(read_until_eof_bytes_cold_impl(insm,
			reinterpret_cast<::std::byte*>(first),
			reinterpret_cast<::std::byte*>(last)));
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(::fast_io::details::streamreflect::has_any_of_pread_operations<instmtype>))
	{
		auto ret{::fast_io::details::pread_until_eof_bytes_cold_impl(insm,first,last)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,ret-first,::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(::fast_io::details::streamreflect::has_any_of_byte_pread_operations<instmtype>))
	{
		auto ret{::fast_io::details::pread_until_eof_cold_impl(insm,first,last)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,last-first,::fast_io::seekdir::cur);
		return ret;
	}
}


template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::byte* read_until_eof_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_until_eof_bytes_underflow_define<instmtype>)
	{
		return read_until_eof_bytes_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_until_eof_bytes_underflow_define<instmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{first,len};
		auto [position,position_in_scatter]{scatter_read_until_eof_bytes_underflow_define(insm,__builtin_addressof(sc),1)};
		if(position)
		{
			return last;
		}
		return first+position_in_scatter;
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>&&sizeof(char_type)==1)
		{
			while(first!=last)
			{
				auto it{read_some_bytes_underflow_define(insm,first,last)};
				if(it==first)
				{
					return it;
				}
				if(it==last)
				{
					return it;
				}
				first=it;
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					first=non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),first);
					ibuffer_set_curr(insm,curr+itdiff);
					return first;
				}
			}
			return first;
		}
		else
		{
			while(first!=last)
			{
				auto it{read_some_bytes_underflow_define(insm,first,last)};
				if(it==first)
				{
					return it;
				}
				first=it;
			}
			return first;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			while(first!=last)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				io_scatter_t sc{first,len};
				::std::size_t sz{::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)};
				first+=sz;
				if(sz==0||first==last)
				{
					return first;
				}
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					first=non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),first);
					ibuffer_set_curr(insm,curr+itdiff);
					return first;
				}
			}
			return first;
		}
		else
		{
			while(first!=last)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				io_scatter_t sc{first,len};
				::std::size_t sz{::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)};
				first+=sz;
				if(!sz||first==last)
				{
					return first;
				}
			}
			return first;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>)
	{
		read_all_bytes_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_read_all_bytes_underflow_define(insm,__builtin_addressof(sc),1);
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_any_of_read_operations<instmtype>)
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		char_type_ptr firstcptr{reinterpret_cast<char_type_ptr>(first)};
		char_type_ptr lastcptr{reinterpret_cast<char_type_ptr>(last)};
		return reinterpret_cast<::std::byte*>(::fast_io::details::read_until_eof_cold_impl(insm,firstcptr,lastcptr));
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(::fast_io::details::streamreflect::has_any_of_byte_pread_operations<instmtype>))
	{
		auto it{::fast_io::details::pread_until_eof_bytes_cold_impl(insm,first,last)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,it-first,::fast_io::seekdir::cur);
		return it;
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(::fast_io::details::streamreflect::has_any_of_pread_operations<instmtype>))
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		char_type_ptr firstcptr{reinterpret_cast<char_type_ptr>(first)};
		char_type_ptr lastcptr{reinterpret_cast<char_type_ptr>(last)};
		auto it{::fast_io::details::pread_until_eof_cold_impl(insm,firstcptr,lastcptr)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,it-firstcptr,::fast_io::seekdir::cur);
		return reinterpret_cast<::std::byte*>(it);
	}
}

}

namespace operations::decay
{

template<typename instmtype>
inline constexpr typename instmtype::input_char_type* read_until_eof_decay(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::operations::decay::read_until_eof_decay(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),first,last);
	}
	else
	{
	if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
	{
		char_type *curr{ibuffer_curr(insm)};
		char_type *ed{ibuffer_end(insm)};
		::std::ptrdiff_t bfddiff{ed-curr};
		::std::ptrdiff_t itdiff{last-first};
		if(itdiff<bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[likely]]
#endif
		{
			non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),first);
			ibuffer_set_curr(insm,curr+itdiff);
			return last;
		}
	}
	return ::fast_io::details::read_until_eof_cold_impl(insm,first,last);
	}
}

template<typename instmtype>
inline constexpr ::std::byte* read_until_eof_bytes_decay(instmtype insm,::std::byte *first,::std::byte *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::operations::decay::read_until_eof_bytes_decay(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),first,last);
	}
	else
	{
	if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>&&sizeof(char_type)==1)
	{
		char_type *curr{ibuffer_curr(insm)};
		char_type *ed{ibuffer_end(insm)};
		::std::ptrdiff_t bfddiff{ed-curr};
		::std::ptrdiff_t itdiff{last-first};
		if(itdiff<bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[likely]]
#endif
		{
			using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type*;
			non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),
				reinterpret_cast<char_type_ptr>(first));
			ibuffer_set_curr(insm,curr+itdiff);
			return last;
		}
	}
	return ::fast_io::details::read_until_eof_bytes_cold_impl(insm,first,last);
	}
}
}

}
