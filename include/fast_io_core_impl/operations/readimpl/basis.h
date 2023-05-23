#pragma once

namespace fast_io
{
namespace details
{

template<typename instmtype>
inline constexpr typename instmtype::input_char_type* pread_some_cold_impl(instmtype insm,
	typename instmtype::input_char_type *first,typename instmtype::input_char_type *last,::fast_io::intfpos_t);

template<typename instmtype>
inline constexpr ::std::byte* pread_some_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last,::fast_io::intfpos_t off);

template<typename instmtype>
inline constexpr void pread_all_cold_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last,::fast_io::intfpos_t off);

template<typename instmtype>
inline constexpr void pread_all_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last,::fast_io::intfpos_t);


template<typename instmtype>
inline constexpr ::std::byte* read_some_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last);

template<typename instmtype>
inline constexpr void read_all_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last);

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr typename instmtype::input_char_type* read_some_cold_impl(instmtype insm,
	typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		return read_some_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<instmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{first,len};
		return ::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
	{
		read_all_underflow_define(insm,first,last);
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{first,len};
		return ::fast_io::scatter_status_one_size(scatter_read_all_underflow_define(insm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr(sizeof(typename instmtype::input_char_type)==1)
		{
			::std::byte* firstptr{reinterpret_cast<::std::byte*>(first)};
			::std::byte* ptr{read_some_bytes_cold_impl(insm,
			firstptr,reinterpret_cast<::std::byte*>(last))};
			return ptr-firstptr+first;
		}
		else
		{
			::std::byte* firstptr{reinterpret_cast<::std::byte*>(first)};
			::std::byte* ptr{read_some_bytes_cold_impl(insm,
			firstptr,reinterpret_cast<::std::byte*>(last))};
			::std::size_t diff{static_cast<::std::size_t>(ptr-firstptr)};
			::std::size_t v{diff/sizeof(char_type)};
			::std::size_t remain{diff%sizeof(char_type)};
			if(remain!=0)
			{
				read_all_bytes_cold_impl(insm,ptr,ptr+remain);
			}
			return first+v;
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
		auto ret{::fast_io::details::pread_some_cold_impl(insm,first,last)};
		::fast_io::details::input_stream_seek_impl(insm,ret-first,::fast_io::seekdir::cur);
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
		auto ret{::fast_io::details::pread_some_cold_impl(insm,first,last)};
		::fast_io::details::input_stream_seek_bytes_impl(insm,(ret-first)*sizeof(char_type),::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::byte* read_some_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>)
	{
		return read_some_bytes_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		io_scatter_t sc{first,len};
		return ::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		return read_some_underflow_define(insm,
			reinterpret_cast<char_type_ptr>(first),
			reinterpret_cast<char_type_ptr>(last));
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<instmtype>)
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type*;
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{reinterpret_cast<char_type_ptr>(first),len};
		return ::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>)
	{
		read_all_bytes_underflow_define(insm,first,last);
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_read_all_bytes_underflow_define(insm,__builtin_addressof(sc),1);
		return last;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		read_all_underflow_define(insm,
			reinterpret_cast<char_type_ptr>(first),
			reinterpret_cast<char_type_ptr>(last));
		return last;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_read_all_bytes_underflow_define(insm,__builtin_addressof(sc),1);
		return last;
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>
	))
	{
		auto ret{::fast_io::details::pread_some_bytes_cold_impl(insm,first,last)};
		::fast_io::details::input_stream_seek_bytes_impl(insm,ret-first,::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>
	))
	{

		auto ret{::fast_io::details::pread_some_bytes_cold_impl(insm,first,last)};
		::fast_io::details::input_stream_seek_impl(insm,ret-first,::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void read_all_cold_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>)
	{
		read_all_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>)
	{
		basic_io_scatter_t<char_type> sc{first,static_cast<::std::size_t>(last-first)};
		scatter_read_all_underflow_define(insm,__builtin_addressof(sc),1);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			while((first=read_some_underflow_define(insm,first,last))!=last)
			{
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
					return;
				}
			}
		}
		else
		{
			while((first=read_some_underflow_define(insm,first,last))!=last);
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
				first+=::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len);
				if(first!=last)
				{
					return;
				}
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
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
				first+=::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len);
				if(first!=last)
				{
					return;
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
		::fast_io::details::input_stream_seek_impl(insm,last-first,::fast_io::seekdir::cur);
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
		::fast_io::details::input_stream_seek_bytes_impl(insm,lastbptr-firstbptr,::fast_io::seekdir::cur);
	}
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void read_all_bytes_cold_impl(instmtype insm,::std::byte *first,::std::byte *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<instmtype>)
	{
		read_all_bytes_underflow_define(insm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<instmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_read_all_bytes_underflow_define(insm,__builtin_addressof(sc),1);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>&&sizeof(char_type)==1)
		{
			while((first=read_some_bytes_underflow_define(insm,first,last))!=last)
			{
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
					return;
				}
			}
		}
		else
		{
			while((first=read_some_bytes_underflow_define(insm,first,last))!=last);
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<instmtype>)
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				io_scatter_t sc{first,len};
				first+=::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len);
				if(first!=last)
				{
					return;
				}
				char_type *curr{ibuffer_curr(insm)};
				char_type *ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
					return;
				}
			}
		}
		else
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				io_scatter_t sc{first,len};
				first+=::fast_io::scatter_status_one_size(scatter_read_some_bytes_underflow_define(insm,__builtin_addressof(sc),1),len);
				if(first!=last)
				{
					return;
				}
			}
		}
	}
	else if constexpr(sizeof(char_type)==1&&
		(::fast_io::details::streamreflect::has_read_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_read_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<instmtype>))
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		char_type_ptr firstcptr{reinterpret_cast<char_type_ptr>(first)};
		char_type_ptr lastcptr{reinterpret_cast<char_type_ptr>(last)};
		::fast_io::details::read_all_cold_impl(insm,firstcptr,lastcptr);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<instmtype>
	))
	{
		::fast_io::details::pread_all_bytes_cold_impl(insm,first,last);
		::fast_io::details::input_stream_seek_bytes_impl(insm,last-first,::fast_io::seekdir::cur);
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_input_or_io_stream_seek_define<instmtype>&&
	(
		::fast_io::details::streamreflect::has_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_pread_some_underflow_define<instmtype>||
		::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<instmtype>
	))
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type*;
		char_type_ptr firstcptr{reinterpret_cast<char_type_ptr>(first)};
		char_type_ptr lastcptr{reinterpret_cast<char_type_ptr>(last)};
		::fast_io::details::pread_all_cold_impl(insm,firstcptr,lastcptr);
		::fast_io::details::input_stream_seek_impl(insm,lastcptr-firstcptr,::fast_io::seekdir::cur);
	}
}

template<typename instmtype>
inline constexpr typename instmtype::input_char_type* read_some_impl(instmtype insm,typename instmtype::input_char_type *first,typename instmtype::input_char_type *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(insm)};
		return ::fast_io::details::read_some_impl(::fast_io::details::input_stream_unlocked_ref_impl(insm),first,last);
	}
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
			ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
			return last;
		}
	}
	return ::fast_io::details::read_some_cold_impl(insm,first,last);
}

template<typename instmtype>
inline constexpr void read_all_impl(instmtype insm,
	typename instmtype::input_char_type *first,
	typename instmtype::input_char_type *last)
{
	if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(insm)};
		return ::fast_io::details::read_all_impl(::fast_io::details::input_stream_unlocked_ref_impl(insm),first,last);
	}
	using char_type = typename instmtype::input_char_type;
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
			ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
			return;
		}
	}
	::fast_io::details::read_all_cold_impl(insm,first,last);
}

template<typename instmtype>
inline constexpr ::std::byte* read_some_bytes_impl(instmtype insm,::std::byte *first,::std::byte *last)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(insm)};
		return ::fast_io::details::read_some_bytes_impl(::fast_io::details::input_stream_unlocked_ref_impl(insm),first,last);
	}
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
			ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
			return last;
		}
	}
	return ::fast_io::details::read_some_bytes_cold_impl(insm,first,last);
}

template<typename instmtype>
inline constexpr void read_all_bytes_impl(instmtype insm,
	::std::byte *first,::std::byte *last)
{
	if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(insm)};
		return ::fast_io::details::read_all_bytes_impl(::fast_io::details::input_stream_unlocked_ref_impl(insm),first,last);
	}
	using char_type = typename instmtype::input_char_type;
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
			ibuffer_set_curr(insm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
			return;
		}
	}
	::fast_io::details::read_all_bytes_cold_impl(insm,first,last);
}

}

}
