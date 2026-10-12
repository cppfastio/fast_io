#pragma once

namespace fast_io
{
namespace details
{
inline void avr_libc_write_common_impl(FILE *fp, char const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	for (auto const e{first + count}; first != e; ++first)
	{
		if (noexcept_call(fp->put, *first, fp)) [[unlikely]]
		{
			throw_posix_error(EINVAL);
		}
	}
}

inline void avr_libc_scatter_write_impl_with_normal_write(FILE *fp, io_scatter_t const *scatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto put_func{fp->put};
	if (put_func == nullptr)
	{
		throw_posix_error(EINVAL);
	}
	for (::std::size_t i{}; i != n; ++i)
	{
		char const *bs{reinterpret_cast<char const *>(scatters[i].base)};
		avr_libc_write_common_impl(fp, bs, scatters[i].len);
	}
}
inline void avr_libc_write_internal_impl(FILE *fp, char const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto put_func{fp->put};
	if (put_func == nullptr)
	{
		throw_posix_error(EINVAL);
	}
	avr_libc_write_common_impl(fp, first, count);
}

inline char *avr_libc_read_internal_impl(FILE *fp, char *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto get_func{fp->get};
	if (get_func == nullptr)
	{
		throw_posix_error(EINVAL);
	}
	for (auto const e{first + count}; first != e; ++first)
	{
		int ret{noexcept_call(getc, fp)};
		if (ret == EOF)
		{
			return first;
		}
		*first = static_cast<char>(static_cast<char unsigned>(ret));
	}
	return first;
}

} // namespace details

template <::std::integral char_type>
inline void write_all_bytes_overflow_define(basic_c_family_io_observer<c_family::emulated_unlocked, char_type> ciob,
											::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::avr_libc_write_internal_impl(ciob.fp, reinterpret_cast<char const *>(first), count);
}

template <::std::integral char_type>
inline void scatter_write_all_bytes_overflow_define(basic_c_family_io_observer<c_family::emulated_unlocked, char_type> ciob,
													io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::avr_libc_scatter_write_impl_with_normal_write(ciob.fp, pscatters, n);
}

template <::std::integral char_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_c_family_io_observer<c_family::emulated_unlocked, char_type> ciob,
													 ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return reinterpret_cast<::std::byte *>(
		::fast_io::details::avr_libc_read_internal_impl(ciob.fp, reinterpret_cast<char *>(first), count));
}

template <::std::integral char_type>
	requires(sizeof(char_type) == sizeof(char))
inline void try_unget(basic_c_family_io_observer<c_family::emulated_unlocked, char_type> ciob, char_type ch) noexcept
{
	noexcept_call(::ungetc, static_cast<int>(ch), ciob.fp);
}

} // namespace fast_io
