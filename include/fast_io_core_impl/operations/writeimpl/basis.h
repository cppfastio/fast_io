#pragma once

namespace fast_io
{
namespace details
{

template<typename outstmtype>
inline constexpr typename outstmtype::output_char_type const* pwrite_some_cold_impl(outstmtype outsm,
	typename outstmtype::output_char_type const *first,typename outstmtype::output_char_type const *last,::fast_io::intfpos_t);

template<typename outstmtype>
inline constexpr ::std::byte const* pwrite_some_bytes_cold_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off);

template<typename outstmtype>
inline constexpr void pwrite_all_cold_impl(outstmtype outsm,typename outstmtype::output_char_type const *first,typename outstmtype::output_char_type const *last,::fast_io::intfpos_t off);

template<typename outstmtype>
inline constexpr void pwrite_all_bytes_cold_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t);


template<typename outstmtype>
inline constexpr ::std::byte const* write_some_bytes_cold_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last);

template<typename outstmtype>
inline constexpr void write_all_bytes_cold_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last);

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr typename outstmtype::output_char_type const* write_some_cold_impl(outstmtype outsm,
	typename outstmtype::output_char_type const *first,typename outstmtype::output_char_type const *last)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_write_some_overflow_define<outstmtype>)
	{
		return write_some_overflow_define(outsm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_overflow_define<outstmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{first,len};
		return ::fast_io::scatter_status_one_size(scatter_write_some_bytes_overflow_define(outsm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_overflow_define<outstmtype>)
	{
		write_all_overflow_define(outsm,first,last);
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_overflow_define<outstmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{first,len};
		return ::fast_io::scatter_status_one_size(scatter_write_all_overflow_define(outsm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<outstmtype>)
	{
		if constexpr(sizeof(typename outstmtype::output_char_type)==1)
		{
			::std::byte const* firstptr{reinterpret_cast<::std::byte const*>(first)};
			::std::byte const* ptr{write_some_bytes_cold_impl(outsm,
			firstptr,reinterpret_cast<::std::byte const*>(last))};
			return ptr-firstptr+first;
		}
		else
		{
			::std::byte const* firstptr{reinterpret_cast<::std::byte const*>(first)};
			::std::byte const* ptr{write_some_bytes_cold_impl(outsm,
			firstptr,reinterpret_cast<::std::byte const*>(last))};
			::std::size_t diff{static_cast<::std::size_t>(ptr-firstptr)};
			::std::size_t v{diff/sizeof(char_type)};
			::std::size_t remain{diff%sizeof(char_type)};
			if(remain!=0)
			{
				write_all_bytes_cold_impl(outsm,ptr,ptr+remain);
			}
			return first+v;
		}
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		auto ret{::fast_io::details::pwrite_some_cold_impl(outsm,first,last)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,ret-first,::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		auto ret{::fast_io::details::pwrite_some_cold_impl(outsm,first,last)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,(ret-first)*sizeof(char_type),::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::byte const* write_some_bytes_cold_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<outstmtype>)
	{
		return write_some_bytes_overflow_define(outsm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<outstmtype>)
	{
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		io_scatter_t sc{first,len};
		return ::fast_io::scatter_status_one_size(scatter_write_some_bytes_overflow_define(outsm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_write_some_overflow_define<outstmtype>)
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type const*;
		return write_some_overflow_define(outsm,
			reinterpret_cast<char_type_const_ptr>(first),
			reinterpret_cast<char_type_const_ptr>(last));
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_scatter_write_some_overflow_define<outstmtype>)
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const*;
		::std::size_t len{static_cast<::std::size_t>(last-first)};
		basic_io_scatter_t<char_type> sc{reinterpret_cast<char_type_const_ptr>(first),len};
		return ::fast_io::scatter_status_one_size(scatter_write_some_bytes_overflow_define(outsm,__builtin_addressof(sc),1),len)+first;
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<outstmtype>)
	{
		write_all_bytes_overflow_define(outsm,first,last);
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<outstmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_write_all_bytes_overflow_define(outsm,__builtin_addressof(sc),1);
		return last;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_write_all_overflow_define<outstmtype>)
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type const*;
		write_all_overflow_define(outsm,
			reinterpret_cast<char_type_const_ptr>(first),
			reinterpret_cast<char_type_const_ptr>(last));
		return last;
	}
	else if constexpr(sizeof(char_type)==1&&
		::fast_io::details::streamreflect::has_scatter_write_all_overflow_define<outstmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_write_all_bytes_overflow_define(outsm,__builtin_addressof(sc),1);
		return last;
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		auto ret{::fast_io::details::pwrite_some_bytes_cold_impl(outsm,first,last)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,ret-first,::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{

		auto ret{::fast_io::details::pwrite_some_bytes_cold_impl(outsm,first,last)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm,ret-first,::fast_io::seekdir::cur);
		return ret;
	}
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void write_all_cold_impl(outstmtype outsm,typename outstmtype::output_char_type const *first,typename outstmtype::output_char_type const *last)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_write_all_overflow_define<outstmtype>)
	{
		write_all_overflow_define(outsm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_overflow_define<outstmtype>)
	{
		basic_io_scatter_t<char_type> sc{first,static_cast<::std::size_t>(last-first)};
		scatter_write_all_overflow_define(outsm,__builtin_addressof(sc),1);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_overflow_define<outstmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>)
		{
			while((first=write_some_overflow_define(outsm,first,last))!=last)
			{
				char_type *curr{obuffer_curr(outsm)};
				char_type *ed{obuffer_end(outsm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					obuffer_set_curr(outsm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
					return;
				}
			}
		}
		else
		{
			while((first=write_some_overflow_define(outsm,first,last))!=last);
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_overflow_define<outstmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>)
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				basic_io_scatter_t<char_type> sc{first,len};
				first+=::fast_io::scatter_status_one_size(scatter_write_some_bytes_overflow_define(outsm,__builtin_addressof(sc),1),len);
				if(first!=last)
				{
					return;
				}
				char_type *curr{obuffer_curr(outsm)};
				char_type *ed{obuffer_end(outsm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					obuffer_set_curr(outsm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
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
				auto [position,position_in_scatter]{scatter_write_some_overflow_define(outsm,__builtin_addressof(sc),1)};
				if(position==1)
				{
					return;
				}
				first+=position_in_scatter;
			}
		}
	}
	else if constexpr(
		(::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<outstmtype>))
	{
		write_all_bytes_cold_impl(outsm,
			reinterpret_cast<::std::byte const*>(first),
			reinterpret_cast<::std::byte const*>(last));
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		::fast_io::details::pwrite_all_bytes_cold_impl(outsm,first,last,0);
		::fast_io::operations::decay::output_stream_seek_decay(outsm,last-first,::fast_io::seekdir::cur);
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		auto firstbptr{reinterpret_cast<::std::byte const*>(first)};
		auto lastbptr{reinterpret_cast<::std::byte const*>(last)};
		::fast_io::details::pwrite_all_bytes_cold_impl(outsm,firstbptr,lastbptr,0);
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,lastbptr-firstbptr,::fast_io::seekdir::cur);
	}
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void write_all_bytes_cold_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<outstmtype>)
	{
		write_all_bytes_overflow_define(outsm,first,last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<outstmtype>)
	{
		io_scatter_t sc{first,static_cast<::std::size_t>(last-first)};
		scatter_write_all_bytes_overflow_define(outsm,__builtin_addressof(sc),1);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<outstmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>&&sizeof(char_type)==1)
		{
			while((first=write_some_bytes_overflow_define(outsm,first,last))!=last)
			{
				char_type *curr{obuffer_curr(outsm)};
				char_type *ed{obuffer_end(outsm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					obuffer_set_curr(outsm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
					return;
				}
			}
		}
		else
		{
			while((first=write_some_bytes_overflow_define(outsm,first,last))!=last);
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<outstmtype>)
	{
		if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>)
		{
			for(;;)
			{
				::std::size_t len{static_cast<::std::size_t>(last-first)};
				io_scatter_t sc{first,len};
				first+=::fast_io::scatter_status_one_size(scatter_write_some_bytes_overflow_define(outsm,__builtin_addressof(sc),1),len);
				if(first!=last)
				{
					return;
				}
				char_type *curr{obuffer_curr(outsm)};
				char_type *ed{obuffer_end(outsm)};
				::std::ptrdiff_t bfddiff{ed-curr};
				::std::ptrdiff_t itdiff{last-first};
				if(itdiff<bfddiff)
				{
					obuffer_set_curr(outsm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
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
				auto [position,position_in_scatter]{scatter_write_some_bytes_overflow_define(outsm,__builtin_addressof(sc),1)};
				if(position==1)
				{
					return;
				}
				first+=position_in_scatter;
			}
		}
	}
	else if constexpr(sizeof(char_type)==1&&
		(::fast_io::details::streamreflect::has_write_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_write_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_write_all_overflow_define<outstmtype>))
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type const*;
		char_type_const_ptr firstcptr{reinterpret_cast<char_type_const_ptr>(first)};
		char_type_const_ptr lastcptr{reinterpret_cast<char_type_const_ptr>(last)};
		::fast_io::details::write_all_cold_impl(outsm,firstcptr,lastcptr);
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_seek_bytes_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_bytes_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>
	))
	{
		::fast_io::details::pwrite_all_bytes_cold_impl(outsm,first,last);
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm,last-first,::fast_io::seekdir::cur);
	}
	else if constexpr(sizeof(char_type)==1&&::fast_io::details::has_output_or_io_stream_seek_define<outstmtype>&&
	(
		::fast_io::details::streamreflect::has_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_all_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_pwrite_some_overflow_define<outstmtype>||
		::fast_io::details::streamreflect::has_scatter_pwrite_some_overflow_define<outstmtype>
	))
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		=
		char_type const*;
		char_type_const_ptr firstcptr{reinterpret_cast<char_type_const_ptr>(first)};
		char_type_const_ptr lastcptr{reinterpret_cast<char_type_const_ptr>(last)};
		::fast_io::details::pwrite_all_cold_impl(outsm,firstcptr,lastcptr);
		::fast_io::operations::decay::output_stream_seek_decay(outsm,lastcptr-firstcptr,::fast_io::seekdir::cur);
	}
}

template<typename outstmtype>
inline constexpr typename outstmtype::output_char_type const* write_some_impl(outstmtype outsm,typename outstmtype::output_char_type const *first,typename outstmtype::output_char_type const *last)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::write_some_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),first,last);
	}
	else
	{
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>)
	{
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};
		::std::ptrdiff_t bfddiff{ed-curr};
		::std::ptrdiff_t itdiff{last-first};
		if(itdiff<bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[likely]]
#endif
		{
			obuffer_set_curr(outsm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
			return last;
		}
	}
	return ::fast_io::details::write_some_cold_impl(outsm,first,last);
	}
}

template<typename outstmtype>
inline constexpr void write_all_impl(outstmtype outsm,
	typename outstmtype::output_char_type const *first,
	typename outstmtype::output_char_type const *last)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::write_all_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),first,last);
	}
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>)
	{
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};
		::std::ptrdiff_t bfddiff{ed-curr};
		::std::ptrdiff_t itdiff{last-first};
		if(itdiff<bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[likely]]
#endif
		{
			obuffer_set_curr(outsm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdiff),curr));
			return;
		}
	}
	::fast_io::details::write_all_cold_impl(outsm,first,last);
}

template<typename outstmtype>
inline constexpr ::std::byte const* write_some_bytes_impl(outstmtype outsm,::std::byte const *first,::std::byte const *last)
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::write_some_bytes_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),first,last);
	}
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>&&sizeof(char_type)==1)
	{
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};
		::std::ptrdiff_t bfddiff{ed-curr};
		::std::ptrdiff_t itdiff{last-first};
		if(itdiff<bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[likely]]
#endif
		{
			using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type const*;
			obuffer_set_curr(outsm,non_overlapped_copy_n(reinterpret_cast<char_type_const_ptr>(first),
				static_cast<::std::size_t>(itdiff),curr));
			return last;
		}
	}
	return ::fast_io::details::write_some_bytes_cold_impl(outsm,first,last);
}

template<typename outstmtype>
inline constexpr void write_all_bytes_impl(outstmtype outsm,
	::std::byte const *first,::std::byte const *last)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::write_all_bytes_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),first,last);
	}
	else
	{
	using char_type = typename outstmtype::output_char_type;
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>&&sizeof(char_type)==1)
	{
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};
		::std::ptrdiff_t bfddiff{ed-curr};
		::std::ptrdiff_t itdiff{last-first};
		if(itdiff<bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[likely]]
#endif
		{
			using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type const*;
			obuffer_set_curr(outsm,non_overlapped_copy_n(reinterpret_cast<char_type_const_ptr>(first),
				static_cast<::std::size_t>(itdiff),curr));
			return;
		}
	}
	::fast_io::details::write_all_bytes_cold_impl(outsm,first,last);
	}
}

template<typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void char_put_cold_impl(outstmtype outstm,
	typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type ch)
{
	if constexpr(::fast_io::details::streamreflect::has_output_stream_char_put_overflow_define<outstmtype>)
	{
		output_stream_char_put_overflow_define(outstm,ch);
	}
	else
	{
		::fast_io::details::write_all_impl(
			outstm,__builtin_addressof(ch),__builtin_addressof(ch)+1);
	}
}

template<typename outstm>
inline constexpr void char_put_impl(outstm outsm,
	typename decltype(::fast_io::manipulators::output_stream_ref(outsm))::output_char_type ch)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<outstm>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::char_put_impl(::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm),ch);
	}
	else
	{
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstm>)
	{
		using char_type = typename outstm::output_char_type;
		char_type *curr{obuffer_curr(outsm)};
		char_type *ed{obuffer_end(outsm)};
		bool condition;
		if constexpr(::fast_io::details::streamreflect::has_obuffer_is_line_buffering_define<outstm>)
		{
			condition=curr<ed;
		}
		else
		{
			condition=curr!=ed;
		}
		if(condition)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif

		{
			*curr=ch;
			obuffer_set_curr(outsm,curr+1);
			return;
		}
	}
	::fast_io::details::char_put_cold_impl(outsm,ch);
	}
}

}

}
