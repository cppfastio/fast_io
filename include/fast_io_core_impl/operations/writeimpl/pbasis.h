#pragma once

namespace fast_io
{
namespace details
{

template <typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr typename outstmtype::output_char_type const *
pwrite_some_cold_impl(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires ::fast_io::operations::decay::defines::pwritable<outstmtype>
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>)
	{
		return pwrite_some_overflow_define(outsm, first, count, off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>)
	{
		::std::size_t len{count};
		basic_io_scatter_t<char_type> sc{first, len};
		return ::fast_io::scatter_status_one_size(
				   scatter_pwrite_some_overflow_define(outsm, __builtin_addressof(sc), 1, off), len) +
			   first;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>)
	{
		pwrite_all_overflow_define(outsm, first, count, off);
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>)
	{
		::std::size_t len{count};
		basic_io_scatter_t<char_type> sc{first, len};
		return ::fast_io::scatter_status_one_size(
				   scatter_pwrite_all_overflow_define(outsm, __builtin_addressof(sc), 1, off), len) +
			   first;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype> ||
					   ::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<
						   outstmtype> ||
					   ::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype> ||
					   ::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		if constexpr (sizeof(typename outstmtype::output_char_type) == 1)
		{
			::std::byte const *firstptr{reinterpret_cast<::std::byte const *>(first)};
			::std::byte const *ptr{
				pwrite_some_bytes_cold_impl(outsm, firstptr, count, off)};
			return ptr - firstptr + first;
		}
		else
		{
			::std::byte const *firstptr{reinterpret_cast<::std::byte const *>(first)};
			::std::byte const *ptr{
				pwrite_some_bytes_cold_impl(outsm, firstptr, count * sizeof(char_type), off)};
			::std::ptrdiff_t ptdf{ptr - firstptr};
			::std::size_t diff{static_cast<::std::size_t>(ptdf)};
			::std::size_t v{diff / sizeof(char_type)};
			::std::size_t remain{diff % sizeof(char_type)};
			if (remain != 0)
			{
				off = ::fast_io::fposoffadd_nonegative(off, ptdf);
				pwrite_all_bytes_cold_impl(outsm, ptr, remain, off);
			}
			return first + v;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm, off, ::fast_io::seekdir::beg);
		auto ret{::fast_io::details::write_some_impl(outsm, first, count)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm, oldoff, ::fast_io::seekdir::beg);
		return ret;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<
							outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<
							outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, off, ::fast_io::seekdir::beg);
		auto ret{::fast_io::details::write_some_impl(outsm, first, count)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, oldoff, ::fast_io::seekdir::beg);
		return ret;
	}
}

template <typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::byte const *pwrite_some_bytes_cold_impl(outstmtype outsm, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires ::fast_io::operations::decay::defines::bytes_pwritable<outstmtype>
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		return pwrite_some_bytes_overflow_define(outsm, first, count, off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		::std::size_t len{count};
		io_scatter_t sc{first, len};
		return ::fast_io::scatter_status_one_size(
				   scatter_pwrite_some_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off), len) +
			   first;
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>)
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type const *;
		return pwrite_some_overflow_define(outsm, reinterpret_cast<char_type_const_ptr>(first), count, off);
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>)
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type const *;
		::std::size_t len{count};
		basic_io_scatter_t<char_type> sc{reinterpret_cast<char_type_const_ptr>(first), len};
		return ::fast_io::scatter_status_one_size(
				   scatter_pwrite_some_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off), len) +
			   first;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		pwrite_all_bytes_overflow_define(outsm, first, count, off);
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		io_scatter_t sc{first, count};
		scatter_pwrite_all_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off);
		return first + count;
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>)
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type const *;
		pwrite_all_overflow_define(outsm, reinterpret_cast<char_type_const_ptr>(first), count, off);
		return first + count;
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>)
	{
		io_scatter_t sc{first, count};
		scatter_pwrite_all_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off);
		return first + count;
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<
							outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<
							outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, off, ::fast_io::seekdir::beg);
		auto ret{::fast_io::details::write_some_bytes_impl(outsm, first, count)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, oldoff, ::fast_io::seekdir::beg);
		return ret;
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   ::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm, off, ::fast_io::seekdir::beg);
		auto ret{::fast_io::details::write_some_impl(outsm, first, count)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm, oldoff, ::fast_io::seekdir::beg);
		return ret;
	}
}

template <typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void pwrite_all_cold_impl(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires ::fast_io::operations::decay::defines::pwritable<outstmtype>
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype>)
	{
		pwrite_all_overflow_define(outsm, first, count, off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>)
	{
		::std::size_t len{count};
		basic_io_scatter_t<char_type> sc{first, len};
		scatter_pwrite_all_overflow_define(outsm, __builtin_addressof(sc), 1, off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype>)
	{
		auto const e{first + count};
		do
		{
			auto nit{pwrite_some_overflow_define(outsm, first, static_cast<::std::size_t>(e - first), off)};
			off = ::fast_io::fposoffadd_nonegative(off, nit - first);
			first = nit;
		} while (first != e);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype>)
	{
		auto const e{first + count};
		for (;;)
		{
			::std::size_t len{static_cast<::std::size_t>(e - first)};
			basic_io_scatter_t<char_type> sc{first, len};
			auto ret{scatter_pwrite_some_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off)};
			auto nit{first + ::fast_io::scatter_status_one_size(ret, len)};
			off = ::fast_io::fposoffadd_nonegative(off, nit - first);
			if (nit != e)
			{
				return;
			}
			first = nit;
		}
	}
	else if constexpr ((::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<
							outstmtype> ||
						::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<
							outstmtype>))
	{
		pwrite_all_bytes_cold_impl(outsm, reinterpret_cast<::std::byte const *>(first), count * sizeof(char_type), off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm, off, ::fast_io::seekdir::beg);
		::fast_io::details::write_all_bytes_impl(outsm, reinterpret_cast<::std::byte const *>(first), count * sizeof(typename outstmtype::output_char_type));
		::fast_io::operations::decay::output_stream_seek_decay(outsm, oldoff, ::fast_io::seekdir::beg);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<
							outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<
							outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, off, ::fast_io::seekdir::beg);
		::fast_io::details::write_all_bytes_impl(outsm, reinterpret_cast<::std::byte const *>(first), count * sizeof(typename outstmtype::output_char_type));
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, oldoff, ::fast_io::seekdir::beg);
	}
}

template <typename outstmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void pwrite_all_bytes_cold_impl(outstmtype outsm, ::std::byte const *first, ::std::size_t count,
												 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires ::fast_io::operations::decay::defines::bytes_pwritable<outstmtype>
{
	using char_type = typename outstmtype::output_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		pwrite_all_bytes_overflow_define(outsm, first, count, off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<outstmtype>)
	{
		io_scatter_t sc{first, count};
		scatter_pwrite_all_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		auto const e{first + count};
		do
		{
			auto nit{pwrite_some_bytes_overflow_define(outsm, first, static_cast<::std::size_t>(e - first), off)};
			off = ::fast_io::fposoffadd_nonegative(off, nit - first);
			first = nit;
		} while (first != e);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<outstmtype>)
	{
		auto const e{first + count};
		do
		{
			::std::size_t len{static_cast<::std::size_t>(e - first)};
			io_scatter_t sc{first, len};
			auto nit =
				first + ::fast_io::scatter_status_one_size(
							scatter_pwrite_some_bytes_overflow_define(outsm, __builtin_addressof(sc), 1, off), len);
			off = ::fast_io::fposoffadd_nonegative(off, nit - first);
			first = nit;
		} while (first != e);
	}
	else if constexpr (sizeof(char_type) == 1 &&
					   (::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<outstmtype>))
	{
		using char_type_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char_type const *;
		char_type_const_ptr firstcptr{reinterpret_cast<char_type_const_ptr>(first)};
		char_type_const_ptr lastcptr{reinterpret_cast<char_type_const_ptr>(first + count)};
		::fast_io::details::pwrite_all_cold_impl(outsm, firstcptr, static_cast<::std::size_t>(lastcptr - firstcptr), off);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_bytes_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<
							outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<
							outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, off, ::fast_io::seekdir::beg);
		::fast_io::details::write_all_bytes_impl(outsm, first, count);
		::fast_io::operations::decay::output_stream_seek_bytes_decay(outsm, oldoff, ::fast_io::seekdir::beg);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_seek_define<outstmtype> &&
					   (::fast_io::operations::decay::defines::has_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_write_some_overflow_define<outstmtype> ||
						::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<outstmtype>))
	{
		auto oldoff{::fast_io::operations::decay::output_stream_seek_decay(outsm, 0, ::fast_io::seekdir::cur)};
		::fast_io::operations::decay::output_stream_seek_decay(outsm, off, ::fast_io::seekdir::beg);
		::fast_io::details::write_all_bytes_impl(outsm, first, count);
		::fast_io::operations::decay::output_stream_seek_decay(outsm, oldoff, ::fast_io::seekdir::beg);
	}
}

template <typename outstmtype>
inline constexpr typename outstmtype::output_char_type const *
pwrite_some_impl(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::pwrite_some_impl(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm), first, count, off);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<outstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(outsm);
		}
		return ::fast_io::details::pwrite_some_cold_impl(outsm, first, count, off);
	}
}

template <typename outstmtype>
inline constexpr void pwrite_all_impl(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::pwrite_all_impl(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm), first, count, off);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<outstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(outsm);
		}
		::fast_io::details::pwrite_all_cold_impl(outsm, first, count, off);
	}
}

template <typename outstmtype>
inline constexpr ::std::byte const *pwrite_some_bytes_impl(outstmtype outsm, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::pwrite_some_bytes_impl(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm), first, count, off);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<outstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(outsm);
		}
		return ::fast_io::details::pwrite_some_bytes_cold_impl(outsm, first, count, off);
	}
}

template <typename outstmtype>
inline constexpr void pwrite_all_bytes_impl(outstmtype outsm, ::std::byte const *first, ::std::size_t count,
											::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(outsm)};
		return ::fast_io::details::pwrite_all_bytes_impl(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(outsm), first, count, off);
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<outstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(outsm);
		}
		::fast_io::details::pwrite_all_bytes_cold_impl(outsm, first, count, off);
	}
}

} // namespace details

} // namespace fast_io
