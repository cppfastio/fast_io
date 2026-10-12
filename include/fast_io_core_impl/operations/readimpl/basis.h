#pragma once

namespace fast_io
{
namespace details
{

template <typename instmtype>
inline constexpr typename instmtype::input_char_type *
pread_some_cold_impl(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count, ::fast_io::intfpos_t)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::preadable<instmtype>
;

template <typename instmtype>
inline constexpr ::std::byte *pread_some_bytes_cold_impl(instmtype insm, ::std::byte *first, ::std::size_t count,
														 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::bytes_preadable<instmtype>
;

template <typename instmtype>
inline constexpr void pread_all_cold_impl(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::preadable<instmtype>
;

template <typename instmtype>
inline constexpr void pread_all_bytes_cold_impl(instmtype insm, ::std::byte *first, ::std::size_t count,
												::fast_io::intfpos_t)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::bytes_preadable<instmtype>
;

template <typename instmtype>
inline constexpr ::std::byte *read_some_bytes_cold_impl(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::bytes_readable<instmtype>
;

template <typename instmtype>
inline constexpr void read_all_bytes_cold_impl(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::bytes_readable<instmtype>
;

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr
typename instmtype::input_char_type *read_some_cold_impl(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::readable<instmtype>
{
	using char_type = typename instmtype::input_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_read_some_underflow_define<instmtype>)
	{
		return read_some_underflow_define(insm, first, count);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_some_underflow_define<instmtype>)
	{
		::std::size_t len{count};
		basic_io_scatter_t<char_type> sc{first, len};
		auto [pos, scpos]{scatter_read_some_underflow_define(insm, __builtin_addressof(sc), 1)};
		if (!pos)
		{
			return first + scpos;
		}
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_read_all_underflow_define<instmtype>)
	{
		read_all_underflow_define(insm, first, count);
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_all_underflow_define<instmtype>)
	{
		::std::size_t len{count};
		basic_io_scatter_t<char_type> sc{first, len};
		scatter_read_all_underflow_define(insm, __builtin_addressof(sc), 1);
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>)
	{
		if constexpr (sizeof(typename instmtype::input_char_type) == 1)
		{
			::std::byte *firstptr{reinterpret_cast<::std::byte *>(first)};
			::std::byte *ptr{read_some_bytes_cold_impl(insm, firstptr, count)};
			return ptr - firstptr + first;
		}
		else
		{
			::std::byte *firstptr{reinterpret_cast<::std::byte *>(first)};
			::std::byte *ptr{read_some_bytes_cold_impl(insm, firstptr, count * sizeof(char_type))};
			::std::size_t diff{static_cast<::std::size_t>(ptr - firstptr)};
			::std::size_t v{diff / sizeof(char_type)};
			::std::size_t remain{diff % sizeof(char_type)};
			if (remain != 0)
			{
				read_all_bytes_cold_impl(insm, ptr, remain);
			}
			return first + v;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_seek_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_operations<instmtype>))
	{
		auto ret{::fast_io::details::pread_some_cold_impl(insm, first, count)};
		::fast_io::operations::decay::input_stream_seek_decay(insm, ret - first, ::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_seek_bytes_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>))
	{
		auto ret{::fast_io::details::pread_some_cold_impl(insm, first, count)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm, (ret - first) * sizeof(char_type),
																	::fast_io::seekdir::cur);
		return ret;
	}
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::byte *read_some_bytes_cold_impl(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::bytes_readable<instmtype>
{
	using char_type = typename instmtype::input_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_read_some_bytes_underflow_define<instmtype>)
	{
		return read_some_bytes_underflow_define(insm, first, count);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		::std::size_t len{count};
		io_scatter_t sc{first, len};
		auto [pos, inscpos] = scatter_read_some_bytes_underflow_define(insm, __builtin_addressof(sc), 1);
		if (!pos)
		{
			return first + inscpos;
		}
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_read_all_bytes_underflow_define<instmtype>)
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type *;
		read_all_bytes_underflow_define(insm, reinterpret_cast<char_type_ptr>(first), count);
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_all_bytes_underflow_define<instmtype>)
	{
		io_scatter_t sc{first, count};
		scatter_read_all_bytes_underflow_define(insm, __builtin_addressof(sc), 1);
		return first + count;
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   (::fast_io::operations::decay::defines::has_any_of_read_operations<instmtype>))
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type *;
		return reinterpret_cast<::std::byte *>(
			read_some_cold_impl(insm, reinterpret_cast<char_type_ptr>(first), count));
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_seek_bytes_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>))
	{
		auto ret{::fast_io::details::pread_some_bytes_cold_impl(insm, first, count, 0)};
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm, ret - first, ::fast_io::seekdir::cur);
		return ret;
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_input_or_io_stream_seek_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_operations<instmtype>))
	{

		auto ret{::fast_io::details::pread_some_bytes_cold_impl(insm, first, count, 0)};
		::fast_io::operations::decay::input_stream_seek_decay(insm, ret - first, ::fast_io::seekdir::cur);
		return ret;
	}
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void read_all_cold_impl(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::readable<instmtype>
{
	using char_type = typename instmtype::input_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_read_all_underflow_define<instmtype>)
	{
		read_all_underflow_define(insm, first, count);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_all_underflow_define<instmtype>)
	{
		basic_io_scatter_t<char_type> sc{first, count};
		scatter_read_all_underflow_define(insm, __builtin_addressof(sc), 1);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_read_some_underflow_define<instmtype>)
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			auto const e{first + count};
			for (decltype(first) it; (it = read_some_underflow_define(insm, first, static_cast<::std::size_t>(e - first))) != e;)
			{
				if (it == first)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
				first = it;
				auto curr{ibuffer_curr(insm)};
				auto ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed - curr};
				::std::ptrdiff_t itdiff{e - first};
				if (itdiff < bfddiff)
				{
					non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff),
										  reinterpret_cast<char_type *>(first));
					ibuffer_set_curr(insm, curr + itdiff);
					return;
				}
			}
		}
		else
		{
			auto const e{first + count};
			for (decltype(first) it; (it = read_some_underflow_define(insm, first, static_cast<::std::size_t>(e - first))) != e; first = it)
			{
				if (it == first)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
			}
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_some_underflow_define<instmtype>)
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			auto const e{first + count};
			for (;;)
			{
				::std::size_t len{static_cast<::std::size_t>(e - first)};
				basic_io_scatter_t<char_type> sc{first, len};
				::std::size_t sz{::fast_io::scatter_status_one_size(
					scatter_read_some_bytes_underflow_define(insm, __builtin_addressof(sc), 1), len)};
				first += sz;
				if (first == e)
				{
					return;
				}
				if (!sz)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
				auto curr{ibuffer_curr(insm)};
				auto ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed - curr};
				::std::ptrdiff_t itdiff{e - first};
				if (itdiff < bfddiff)
				{
					non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff),
										  reinterpret_cast<char_type *>(first));
					ibuffer_set_curr(insm, curr + itdiff);
					return;
				}
			}
		}
		else
		{
			auto const e{first + count};
			for (;;)
			{
				::std::size_t len{static_cast<::std::size_t>(e - first)};
				basic_io_scatter_t<char_type> sc{first, len};
				::std::size_t sz{::fast_io::scatter_status_one_size(
					scatter_read_some_bytes_underflow_define(insm, __builtin_addressof(sc), 1), len)};
				first += sz;
				if (first == e)
				{
					return;
				}
				if (!sz)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
			}
		}
	}
	else if constexpr ((::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>))
	{
		read_all_bytes_cold_impl(insm, reinterpret_cast<::std::byte *>(first), count * sizeof(char_type));
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_seek_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_operations<instmtype>))
	{
		::fast_io::details::pread_all_bytes_cold_impl(insm, first, count);
		::fast_io::operations::decay::input_stream_seek_decay(insm, count, ::fast_io::seekdir::cur);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_seek_bytes_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>))
	{
		auto firstbptr{reinterpret_cast<::std::byte *>(first)};
		auto lastbptr{reinterpret_cast<::std::byte *>(first + count)};
		::fast_io::details::pread_all_bytes_cold_impl(insm, firstbptr, static_cast<::std::size_t>(lastbptr - firstbptr));
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm, lastbptr - firstbptr,
																	::fast_io::seekdir::cur);
	}
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void read_all_bytes_cold_impl(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires ::fast_io::operations::decay::defines::bytes_readable<instmtype>
{
	using char_type = typename instmtype::input_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_read_all_bytes_underflow_define<instmtype>)
	{
		read_all_bytes_underflow_define(insm, first, count);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_all_bytes_underflow_define<instmtype>)
	{
		io_scatter_t sc{first, count};
		scatter_read_all_bytes_underflow_define(insm, __builtin_addressof(sc), 1);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype> &&
					  sizeof(char_type) == 1)
		{
			auto const e{first + count};
			for (decltype(first) it; (it = read_some_bytes_underflow_define(insm, first, static_cast<::std::size_t>(e - first))) != e;)
			{
				if (it == first)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
				first = it;
				auto curr{ibuffer_curr(insm)};
				auto ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed - curr};
				::std::ptrdiff_t itdiff{e - first};
				if (itdiff < bfddiff)
				{
					non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff),
										  reinterpret_cast<char_type *>(first));
					ibuffer_set_curr(insm, curr + itdiff);
					return;
				}
			}
		}
		else
		{
			auto const e{first + count};
			for (decltype(first) it; (it = read_some_bytes_underflow_define(insm, first, static_cast<::std::size_t>(e - first))) != e; first = it)
			{
				if (it == first)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
			}
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_read_some_bytes_underflow_define<instmtype>)
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			auto const e{first + count};
			for (;;)
			{
				::std::size_t len{static_cast<::std::size_t>(e - first)};
				io_scatter_t sc{first, len};
				::std::size_t sz{::fast_io::scatter_status_one_size(
					scatter_read_some_bytes_underflow_define(insm, __builtin_addressof(sc), 1), len)};
				first += sz;
				if (first == e)
				{
					return;
				}
				if (!sz)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
				auto curr{ibuffer_curr(insm)};
				auto ed{ibuffer_end(insm)};
				::std::ptrdiff_t bfddiff{ed - curr};
				::std::ptrdiff_t itdiff{e - first};
				if (itdiff < bfddiff)
				{
					non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff),
										  reinterpret_cast<char_type *>(first));
					ibuffer_set_curr(insm, curr + itdiff);
					return;
				}
			}
		}
		else
		{
			auto const e{first + count};
			for (;;)
			{
				::std::size_t len{static_cast<::std::size_t>(e - first)};
				io_scatter_t sc{first, len};
				::std::size_t sz{::fast_io::scatter_status_one_size(
					scatter_read_some_bytes_underflow_define(insm, __builtin_addressof(sc), 1), len)};
				first += sz;
				if (first == e)
				{
					return;
				}
				if (!sz)
				{
					::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
				}
			}
		}
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_any_of_read_operations<instmtype>)
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type *;
		char_type_ptr firstcptr{reinterpret_cast<char_type_ptr>(first)};
		char_type_ptr lastcptr{reinterpret_cast<char_type_ptr>(first + count)};
		::fast_io::details::read_all_cold_impl(insm, firstcptr, static_cast<::std::size_t>(lastcptr - firstcptr));
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_seek_bytes_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<instmtype>))
	{
		::fast_io::details::pread_all_bytes_cold_impl(insm, first, count);
		::fast_io::operations::decay::input_stream_seek_bytes_decay(insm, count, ::fast_io::seekdir::cur);
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_input_or_io_stream_seek_define<instmtype> &&
					   (::fast_io::operations::decay::defines::has_any_of_pread_operations<instmtype>))
	{
		using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type *;
		char_type_ptr firstcptr{reinterpret_cast<char_type_ptr>(first)};
		char_type_ptr lastcptr{reinterpret_cast<char_type_ptr>(first + count)};
		::fast_io::details::pread_all_cold_impl(insm, firstcptr, static_cast<::std::size_t>(lastcptr - firstcptr));
		::fast_io::operations::decay::input_stream_seek_decay(insm, lastcptr - firstcptr, ::fast_io::seekdir::cur);
	}
}

template <typename instmtype>
inline constexpr typename instmtype::input_char_type *
read_some_impl(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::read_some_impl(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),
												  first, count);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			auto curr{ibuffer_curr(insm)};
			auto ed{ibuffer_end(insm)};
			::std::ptrdiff_t bfddiff{ed - curr};
			::std::ptrdiff_t itdiff{static_cast<::std::ptrdiff_t>(count)};
			if (itdiff < bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[likely]]
#endif
			{
				non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff), first);
				ibuffer_set_curr(insm, curr + itdiff);
				return first + count;
			}
		}
		return ::fast_io::details::read_some_cold_impl(insm, first, count);
	}
}

template <typename instmtype>
inline constexpr void read_all_impl(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::read_all_impl(::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm),
												 first, count);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
		{
			auto curr{ibuffer_curr(insm)};
			auto ed{ibuffer_end(insm)};
			::std::ptrdiff_t bfddiff{ed - curr};
			::std::ptrdiff_t itdiff{static_cast<::std::ptrdiff_t>(count)};
			if (itdiff < bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[likely]]
#endif
			{
				non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff), first);
				ibuffer_set_curr(insm, curr + itdiff);
				return;
			}
		}
		::fast_io::details::read_all_cold_impl(insm, first, count);
	}
}

template <typename instmtype>
inline constexpr ::std::byte *read_some_bytes_impl(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	using char_type = typename instmtype::input_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::read_some_bytes_impl(
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm), first, count);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype> &&
					  sizeof(char_type) == 1)
		{
			auto curr{ibuffer_curr(insm)};
			auto ed{ibuffer_end(insm)};
			::std::ptrdiff_t bfddiff{ed - curr};
			::std::ptrdiff_t itdiff{static_cast<::std::ptrdiff_t>(count)};
			if (itdiff < bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[likely]]
#endif
			{
				using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= char_type *;
				non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff), reinterpret_cast<char_type_ptr>(first));
				ibuffer_set_curr(insm, curr + itdiff);
				return first + count;
			}
		}
		return ::fast_io::details::read_some_bytes_cold_impl(insm, first, count);
	}
}

template <typename instmtype>
inline constexpr void read_all_bytes_impl(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(insm)};
		return ::fast_io::details::read_all_bytes_impl(
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(insm), first, count);
	}
	else
	{
		using char_type = typename instmtype::input_char_type;
		if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype> &&
					  sizeof(char_type) == 1)
		{
			auto curr{ibuffer_curr(insm)};
			auto ed{ibuffer_end(insm)};
			::std::ptrdiff_t bfddiff{ed - curr};
			::std::ptrdiff_t itdiff{static_cast<::std::ptrdiff_t>(count)};
			if (itdiff < bfddiff)
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[likely]]
#endif
			{
				using char_type_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= char_type *;
				non_overlapped_copy_n(curr, static_cast<::std::size_t>(itdiff), reinterpret_cast<char_type_ptr>(first));
				ibuffer_set_curr(insm, curr + itdiff);
				return;
			}
		}
		::fast_io::details::read_all_bytes_cold_impl(insm, first, count);
	}
}

} // namespace details

} // namespace fast_io
