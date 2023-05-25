#pragma once

namespace fast_io
{

namespace details
{


template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr typename instmtype::input_char_type* read_until_eof_cold_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
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
	else if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			for(decltype(first) it;(it=read_some_underflow_define(insm,first,last))!=last;)
			{
				if(it==first)
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
			for(decltype(first) it;(it=read_some_underflow_define(insm,first,last))!=last;first=it)
			{
				if(it==first)
				{
					return it;
				}
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
				::std::size_t sz{::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)};
				first+=sz;
				if(first==last)
				{
					return first;
				}
				if(!sz)
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
		}
		else
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				basic_io_scatter_t<char_type> sc{first,len};
				::std::size_t sz{::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)};
				first+=sz;
				if(first==last)
				{
					return first;
				}
				if(!sz)
				{
					return first;
				}
			}
			return last;
		}
	}
	else if constexpr(
		(::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>))
	{
		read_all_bytes_cold_impl(insm,
			reinterpret_cast<::std::byte*>(first),
			reinterpret_cast<::std::byte*>(last));
		return last;
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>
	))
	{
		auto ret{::fast_io::details::pread_until_eof_bytes_cold_impl(insm,first,last)};
		::fast_io::operations::decay::input_stream_seek_decay(insm,last-first,::fast_io::seekdir::cur);

	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>
	))
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
inline constexpr typename instmtype::input_char_type * read_until_eof_cold_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
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
	else if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			for(decltype(first) it;(it=read_some_underflow_define(insm,first,last))!=last;)
			{
				if(it==first)
				{
					::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file);
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
			return last;
		}
		else
		{
			for(decltype(first) it;(it=read_some_underflow_define(insm,first,last))!=last;first=it)
			{
				if(it==first)
				{
					return first;
				}
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
				::std::size_t sz{::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)};
				first+=sz;
				if(first==last)
				{
					return;
				}
				if(!sz)
				{
					::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file);
				}
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdiff),first);
					ibuffer_set_curr(insm,curr+itdiff);
					return;
				}
			}
		}
		else
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				basic_io_scatter_t<char_type> sc{first,len};
				::std::size_t sz{::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)};
				first+=sz;
				if(first==last)
				{
					return;
				}
				if(!sz)
				{
					::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file);
				}
			}
		}
	}
	else if constexpr(
		(::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>))
	{
		read_all_bytes_cold_impl(insm,
			reinterpret_cast<::std::byte*>(first),
			reinterpret_cast<::std::byte*>(last));
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>
	))
	{
		::fast_io::details::pread_all_bytes_cold_impl(insm,first,last);
		::fast_io::operations::decay::input_stream_seek_decay(insm,last-first,::fast_io::seekdir::cur);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>
	))
	{
		auto firstbptr{reinterpret_cast<::std::byte*>(first)};
		auto lastbptr{reinterpret_cast<::std::byte*>(last)};
		::fast_io::details::pread_all_bytes_cold_impl(insm,firstbptr,lastbptr);
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm,lastbptr-firstbptr,::fast_io::seekdir::cur);
	}
}



}

}
