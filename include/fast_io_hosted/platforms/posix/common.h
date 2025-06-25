#pragma once

namespace fast_io
{

namespace details
{

inline ::std::byte *posix_read_bytes_impl(int fd, ::std::byte *first, ::std::byte *last)
{
#if defined(__linux__) && defined(__NR_read)
	auto ret{system_call<__NR_read, ::std::ptrdiff_t>(fd, first, static_cast<::std::size_t>(last - first))};
	::fast_io::linux_system_call_throw_error(ret);
#else
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	auto ret{
		::fast_io::noexcept_call(::_read, fd, first,
								 static_cast<::std::uint_least32_t>(
									 ::fast_io::details::read_write_bytes_compute<::std::int_least32_t>(first, last)))};
#else
	auto ret{::fast_io::noexcept_call(::read, fd, first, static_cast<::std::size_t>(last - first))};
#endif
	if (ret < 0)
	{
		::fast_io::throw_posix_error();
	}
#endif
	return first + ret;
}

inline ::std::byte const *posix_write_bytes_impl(int fd, ::std::byte const *first, ::std::byte const *last)
{
#if defined(__linux__) && defined(__NR_write)
	auto ret{system_call<__NR_write, ::std::ptrdiff_t>(fd, first, static_cast<::std::size_t>(last - first))};
	::fast_io::linux_system_call_throw_error(ret);
#else
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	auto ret{
		::fast_io::noexcept_call(::_write, fd, first,
								 static_cast<::std::uint_least32_t>(
									 ::fast_io::details::read_write_bytes_compute<::std::int_least32_t>(first, last)))};
#else
	auto ret{::fast_io::noexcept_call(::write, fd, first, static_cast<::std::size_t>(last - first))};
#endif
	if (ret < 0)
	{
		::fast_io::throw_posix_error();
	}
#endif
	return first + ret;
}

#ifdef __MSDOS__

extern unsigned my_dos_read(int, void *, unsigned, unsigned *) noexcept __asm__("__dos_read");
extern unsigned my_dos_write(int, void const *, unsigned, unsigned *) noexcept __asm__("__dos_write");

inline ::std::byte *posix_dos_read_bytes_impl(int fd, ::std::byte *first, ::std::byte *last)
{
	unsigned ret;
	if (my_dos_read(fd, first, ::fast_io::details::read_write_bytes_compute<unsigned>(first, last), __builtin_addressof(ret)))
	{
		throw_posix_error();
	}
	return first + ret;
}

inline ::std::byte const *posix_dos_write_bytes_impl(int fd, ::std::byte const *first, ::std::byte const *last)
{
	unsigned ret;
	if (my_dos_write(fd, first, ::fast_io::details::read_write_bytes_compute<unsigned>(first, last), __builtin_addressof(ret)))
	{
		throw_posix_error();
	}
	return first + ret;
}
#endif

} // namespace details

template <::fast_io::posix_family family, ::std::integral char_type>
inline ::std::byte *read_some_bytes_underflow_define(::fast_io::basic_posix_family_io_observer<family, char_type> piob,
													 ::std::byte *first, ::std::byte *last)
{
#ifdef __MSDOS__
	if constexpr (family == ::fast_io::posix_family::dos)
	{
		return ::fast_io::details::posix_dos_read_bytes_impl(piob.fd, first, last);
	}
	else
#endif
	{
		return ::fast_io::details::posix_read_bytes_impl(piob.fd, first, last);
	}
}

template <::fast_io::posix_family family, ::std::integral char_type>
inline ::std::byte const *write_some_bytes_overflow_define(::fast_io::basic_posix_family_io_observer<family, char_type> piob,
														   ::std::byte const *first, ::std::byte const *last)
{
#ifdef __MSDOS__
	if constexpr (family == ::fast_io::posix_family::dos)
	{
		return ::fast_io::details::posix_dos_write_bytes_impl(piob.fd, first, last);
	}
	else
#endif
	{
		return ::fast_io::details::posix_write_bytes_impl(piob.fd, first, last);
	}
}

} // namespace fast_io
