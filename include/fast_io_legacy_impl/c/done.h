#pragma once

namespace fast_io
{

namespace details
{

#if defined(__CYGWIN__)
#if (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern ::std::size_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
	__cdecl
#endif
	my_cygwin_fwrite_unlocked(void const *__restrict buffer, ::std::size_t size, ::std::size_t count,
							  FILE *__restrict) noexcept __asm__("fwrite_unlocked");

#if (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern ::std::size_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
	__cdecl
#endif
	my_cygwin_fread_unlocked(void *__restrict buffer, ::std::size_t size, ::std::size_t count,
							 FILE *__restrict) noexcept __asm__("fread_unlocked");
#endif

inline ::std::size_t c_fwrite_unlocked_impl(void const *__restrict begin, ::std::size_t type_size, ::std::size_t count,
											FILE *__restrict fp)
{
	if (count == 0)
	{
		return 0;
	}
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	struct _reent rent{};
	::std::size_t written_count{
#if defined(__IMPL_UNLOCKED__)
		noexcept_call(_fwrite_unlocked_r, __builtin_addressof(rent), begin, type_size, count, fp)
#else
		noexcept_call(_fwrite_r, __builtin_addressof(rent), begin, type_size, count, fp)
#endif
	};
	if (!written_count) [[unlikely]]
	{
		throw_posix_error(rent._errno);
	}
#else

#pragma push_macro("FAST_IO_FWRITE_UNLOCKED")
#pragma push_macro("FAST_IO_FWRITE")
#pragma push_macro("FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED")
#pragma push_macro("FAST_IO_FWRITE_UNLOCKED_IMPL")

#undef FAST_IO_FWRITE_UNLOCKED
#undef FAST_IO_FWRITE
#undef FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED
#undef FAST_IO_FWRITE_UNLOCKED_IMPL

#ifdef __has_builtin
#if __has_builtin(__builtin_fwrite_unlocked)
#define FAST_IO_FWRITE_UNLOCKED(...) __builtin_fwrite_unlocked(__VA_ARGS__)
#endif
#endif
#ifndef FAST_IO_FWRITE_UNLOCKED
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FWRITE_UNLOCKED(...) ::fwrite_unlocked(__VA_ARGS__)
#elif defined(fwrite_unlocked)
#define FAST_IO_FWRITE_UNLOCKED(...) fwrite_unlocked(__VA_ARGS__)
#else
#define FAST_IO_FWRITE_UNLOCKED(...) ::fast_io::noexcept_call(::fwrite_unlocked, __VA_ARGS__)
#endif
#endif

#ifdef __has_builtin
#if __has_builtin(__builtin_fwrite)
#define FAST_IO_FWRITE(...) __builtin_fwrite(__VA_ARGS__)
#endif
#endif
#ifndef FAST_IO_FWRITE
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FWRITE(...) ::fwrite(__VA_ARGS__)
#elif defined(fwrite)
#define FAST_IO_FWRITE(...) fwrite(__VA_ARGS__)
#else
#define FAST_IO_FWRITE(...) ::fast_io::noexcept_call(::fwrite, __VA_ARGS__)
#endif
#endif

#if defined(__BIONIC__)
#if defined(__BIONIC_AVAILABILITY_GUARD)
#if __BIONIC_AVAILABILITY_GUARD(28)
#define FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED 1
#endif
#endif
#elif defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED 1
#elif defined(__USE_MISC) || defined(__BSD_VISIBLE)
#define FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED 1
#endif

#if defined(__CYGWIN__)
#define FAST_IO_FWRITE_UNLOCKED_IMPL(...) my_cygwin_fwrite_unlocked(__VA_ARGS__)
#elif defined(FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED)
#define FAST_IO_FWRITE_UNLOCKED_IMPL(...) FAST_IO_FWRITE_UNLOCKED(__VA_ARGS__)
#else
#define FAST_IO_FWRITE_UNLOCKED_IMPL(...) FAST_IO_FWRITE(__VA_ARGS__)
#endif

	::std::size_t const written_count{FAST_IO_FWRITE_UNLOCKED_IMPL(begin, type_size, count, fp)};

#pragma pop_macro("FAST_IO_FWRITE_UNLOCKED_IMPL")
#pragma pop_macro("FAST_IO_PLATFORM_SUPPORTS_FWRITE_UNLOCKED")
#pragma pop_macro("FAST_IO_FWRITE")
#pragma pop_macro("FAST_IO_FWRITE_UNLOCKED")
	if (!written_count) [[unlikely]]
	{
		throw_posix_error();
	}

#endif
	return written_count;
}

inline ::std::size_t c_fread_unlocked_impl(void *__restrict begin, ::std::size_t type_size, ::std::size_t count,
										   FILE *__restrict fp)
{
	if (fp == stdin)
	{
		::fast_io::noexcept_call(::fflush, stdout);
	}
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	struct _reent rent{};
	::std::size_t read_count{
#if defined(__IMPL_UNLOCKED__)
		noexcept_call(_fread_unlocked_r, __builtin_addressof(rent), begin, type_size, count, fp)
#else
		noexcept_call(_fread_r, __builtin_addressof(rent), begin, type_size, count, fp)
#endif
	};
	if (read_count == 0) [[unlikely]]
	{
		if (rent._errno)
		{
			throw_posix_error(rent._errno);
		}
	}
#else

#pragma push_macro("FAST_IO_FREAD_UNLOCKED")
#pragma push_macro("FAST_IO_FREAD")
#pragma push_macro("FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED")
#pragma push_macro("FAST_IO_FREAD_UNLOCKED_IMPL")

#undef FAST_IO_FREAD_UNLOCKED
#undef FAST_IO_FREAD
#undef FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED
#undef FAST_IO_FREAD_UNLOCKED_IMPL

// Define FAST_IO_FREAD_UNLOCKED
#ifdef __has_builtin
#if __has_builtin(__builtin_fread_unlocked)
#define FAST_IO_FREAD_UNLOCKED(...) __builtin_fread_unlocked(__VA_ARGS__)
#endif
#endif
#ifndef FAST_IO_FREAD_UNLOCKED
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FREAD_UNLOCKED(...) ::fread_unlocked(__VA_ARGS__)
#elif defined(fread_unlocked)
#define FAST_IO_FREAD_UNLOCKED(...) fread_unlocked(__VA_ARGS__)
#else
#define FAST_IO_FREAD_UNLOCKED(...) ::fast_io::noexcept_call(::fread_unlocked, __VA_ARGS__)
#endif
#endif

// Define FAST_IO_FREAD
#ifdef __has_builtin
#if __has_builtin(__builtin_fread)
#define FAST_IO_FREAD(...) __builtin_fread(__VA_ARGS__)
#endif
#endif
#ifndef FAST_IO_FREAD
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FREAD(...) ::fread(__VA_ARGS__)
#elif defined(fread)
#define FAST_IO_FREAD(...) fread(__VA_ARGS__)
#else
#define FAST_IO_FREAD(...) ::fast_io::noexcept_call(::fread, __VA_ARGS__)
#endif
#endif

// Detect whether platform supports fread_unlocked
#if defined(__BIONIC__)
#if defined(__BIONIC_AVAILABILITY_GUARD)
#if __BIONIC_AVAILABILITY_GUARD(28)
#define FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED 1
#endif
#endif
#elif defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED 1
#elif defined(__USE_MISC) || defined(__BSD_VISIBLE)
#define FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED 1
#endif

// Define the unified dispatch macro
#if defined(__CYGWIN__)
#define FAST_IO_FREAD_UNLOCKED_IMPL(...) my_cygwin_fread_unlocked(__VA_ARGS__)
#elif defined(FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED)
#define FAST_IO_FREAD_UNLOCKED_IMPL(...) FAST_IO_FREAD_UNLOCKED(__VA_ARGS__)
#else
#define FAST_IO_FREAD_UNLOCKED_IMPL(...) FAST_IO_FREAD(__VA_ARGS__)
#endif

	::std::size_t const read_count{FAST_IO_FREAD_UNLOCKED_IMPL(begin, type_size, count, fp)};

#pragma pop_macro("FAST_IO_FREAD_UNLOCKED_IMPL")
#pragma pop_macro("FAST_IO_PLATFORM_SUPPORTS_FREAD_UNLOCKED")
#pragma pop_macro("FAST_IO_FREAD")
#pragma pop_macro("FAST_IO_FREAD_UNLOCKED")
	if (read_count == 0) [[unlikely]]
	{
		if (
#if defined(__CYGWIN__)
			__sferror(fp)
#elif defined(__USE_MISC) || defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)
#if FAST_IO_HAS_BUILTIN(__builtin_ferror_unlocked)
			__builtin_ferror_unlocked(fp)
#else
			ferror_unlocked(fp)
#endif
#elif !defined(ferror) && FAST_IO_HAS_BUILTIN(__builtin_ferror)
			__builtin_ferror(fp)
#else
			ferror(fp)
#endif
		)
			throw_posix_error();
	}
#endif
	return read_count;
}

inline ::std::size_t c_fwrite_impl(void const *__restrict begin, ::std::size_t type_size, ::std::size_t count,
								   FILE *__restrict fp)
{
	if (count == 0)
	{
		return 0;
	}
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	struct _reent rent{};
	::std::size_t written_count{noexcept_call(_fwrite_r, __builtin_addressof(rent), begin, type_size, count, fp)};
	if (!written_count) [[unlikely]]
	{
		throw_posix_error(rent._errno);
	}
#else
#pragma push_macro("FAST_IO_FWRITE")
#undef FAST_IO_FWRITE

#ifdef __has_builtin
#if __has_builtin(__builtin_fwrite)
#define FAST_IO_FWRITE(...) __builtin_fwrite(__VA_ARGS__)
#endif
#endif
#ifndef FAST_IO_FWRITE
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FWRITE(...) ::fwrite(__VA_ARGS__)
#elif defined(fwrite)
#define FAST_IO_FWRITE(...) fwrite(__VA_ARGS__)
#else
#define FAST_IO_FWRITE(...) ::fast_io::noexcept_call(::fwrite, __VA_ARGS__)
#endif
#endif

	::std::size_t const written_count{FAST_IO_FWRITE(begin, type_size, count, fp)};

#pragma pop_macro("FAST_IO_FWRITE")
	if (!written_count) [[unlikely]]
	{
		throw_posix_error();
	}
#endif
	return written_count;
}

inline ::std::size_t c_fread_impl(void *__restrict begin, ::std::size_t type_size, ::std::size_t count,
								  FILE *__restrict fp)
{
	if (fp == stdin)
	{
		::fast_io::noexcept_call(::fflush, stdout);
	}
#if defined(__NEWLIB__)
	struct _reent rent{};
	::std::size_t read_count{noexcept_call(_fread_r, __builtin_addressof(rent), begin, type_size, count, fp)};
	if (read_count == 0) [[unlikely]]
	{
		if (rent._errno)
		{
			throw_posix_error(rent._errno);
		}
	}
#else
#pragma push_macro("FAST_IO_FREAD")
#undef FAST_IO_FREAD

#ifdef __has_builtin
#if __has_builtin(__builtin_fread)
#define FAST_IO_FREAD(...) __builtin_fread(__VA_ARGS__)
#endif
#endif
#ifndef FAST_IO_FREAD
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FREAD(...) ::fread(__VA_ARGS__)
#elif defined(fread)
#define FAST_IO_FREAD(...) fread(__VA_ARGS__)
#else
#define FAST_IO_FREAD(...) ::fast_io::noexcept_call(::fread, __VA_ARGS__)
#endif
#endif

	::std::size_t const read_count{FAST_IO_FREAD(begin, type_size, count, fp)};

#pragma pop_macro("FAST_IO_FREAD")
	if (read_count == 0) [[unlikely]]
	{
		if (
#if !defined(ferror) && FAST_IO_HAS_BUILTIN(__builtin_ferror)
			__builtin_ferror(fp)
#else
			ferror(fp)
#endif
		)
			throw_posix_error();
	}
#endif
	return read_count;
}

inline ::std::byte *c_read_some_bytes_impl(FILE *__restrict fp, ::std::byte *first, ::std::byte *last)
{
	return c_fread_impl(first, 1, static_cast<::std::size_t>(last - first), fp) + first;
}

inline ::std::byte *c_unlocked_read_some_bytes_impl(FILE *__restrict fp, ::std::byte *first, ::std::byte *last)
{
	return c_fread_unlocked_impl(first, 1, static_cast<::std::size_t>(last - first), fp) + first;
}

inline ::std::byte const *c_write_some_bytes_impl(FILE *__restrict fp, ::std::byte const *first,
												  ::std::byte const *last)
{
	return c_fwrite_impl(first, 1, static_cast<::std::size_t>(last - first), fp) + first;
}

inline ::std::byte const *c_unlocked_write_some_bytes_impl(FILE *__restrict fp, ::std::byte const *first,
														   ::std::byte const *last)
{
	return c_fwrite_unlocked_impl(first, 1, static_cast<::std::size_t>(last - first), fp) + first;
}

} // namespace details

template <::fast_io::c_family family, ::std::integral char_type>
inline ::std::byte *read_some_bytes_underflow_define(::fast_io::basic_c_family_io_observer<family, char_type> ciob,
													 ::std::byte *first, ::std::byte *last)
{
	if constexpr (family == ::fast_io::c_family::unlocked || family == ::fast_io::c_family::emulated_unlocked)
	{
		return ::fast_io::details::c_unlocked_read_some_bytes_impl(ciob.fp, first, last);
	}
	else
	{
		return ::fast_io::details::c_read_some_bytes_impl(ciob.fp, first, last);
	}
}

template <::fast_io::c_family family, ::std::integral char_type>
inline ::std::byte const *
write_some_bytes_overflow_define(::fast_io::basic_c_family_io_observer<family, char_type> ciob,
								 ::std::byte const *first, ::std::byte const *last)
{
	if constexpr (family == ::fast_io::c_family::unlocked || family == ::fast_io::c_family::emulated_unlocked)
	{
		return ::fast_io::details::c_unlocked_write_some_bytes_impl(ciob.fp, first, last);
	}
	else
	{
		return ::fast_io::details::c_write_some_bytes_impl(ciob.fp, first, last);
	}
}

} // namespace fast_io
