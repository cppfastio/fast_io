#pragma once

namespace fast_io
{

namespace details
{

#if defined(__CYGWIN__)
#if (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern std::size_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
__cdecl
#endif
my_cygwin_fwrite_unlocked(void const* __restrict buffer,std::size_t size,std::size_t count,FILE* __restrict) noexcept __asm__("fwrite_unlocked");

#if (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern std::size_t 
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
__cdecl
#endif
my_cygwin_fread_unlocked(void* __restrict buffer,std::size_t size,std::size_t count,FILE* __restrict) noexcept __asm__("fread_unlocked");
#endif

inline std::size_t c_fwrite_unlocked_impl(void const* __restrict begin,std::size_t type_size,std::size_t count,FILE* __restrict fp)
{
	if(count==0)
		return 0;
#if defined(__NEWLIB__)&&!defined(__CYGWIN__)
	struct _reent rent{};
	std::size_t written_count{
#if defined(__IMPL_UNLOCKED__)
	noexcept_call(_fwrite_unlocked_r,__builtin_addressof(rent),begin,type_size,count,fp)
#else
	noexcept_call(_fwrite_r,__builtin_addressof(rent),begin,type_size,count,fp)
#endif
	};
	if(!written_count)[[unlikely]]
		throw_posix_error(rent._errno);
#else
	std::size_t written_count{
#if defined(__CYGWIN__)
	my_cygwin_fwrite_unlocked(begin,type_size,count,fp)
#elif (defined(__USE_MISC) || defined(__BSD_VISIBLE)) && (!defined(__BIONIC__) || (defined(__USE_BSD) ))
#if !defined(fwrite_unlocked) && defined(__has_builtin)
#if __has_builtin(__builtin_fwrite_unlocked)
	__builtin_fwrite_unlocked(begin,type_size,count,fp)
#else
	fwrite_unlocked(begin,type_size,count,fp)
#endif
#else
	fwrite_unlocked(begin,type_size,count,fp)
#endif
#elif !defined(fwrite)&&defined(__has_builtin)
#if __has_builtin(__builtin_fwrite)
	__builtin_fwrite(begin,type_size,count,fp)
#else
	fwrite(begin,type_size,count,fp)
#endif
#else
	fwrite(begin,type_size,count,fp)
#endif
	};
	if(!written_count)[[unlikely]]
		throw_posix_error();
#endif
	return written_count;
}

inline std::size_t c_fread_unlocked_impl(void* __restrict begin,std::size_t type_size,std::size_t count,FILE* __restrict fp)
{
	if(fp==stdin)
	{
		::fast_io::noexcept_call(::fflush,stdout);
	}
#if defined(__NEWLIB__)&&!defined(__CYGWIN__)
	struct _reent rent{};
	std::size_t read_count{
#if defined(__IMPL_UNLOCKED__)
	noexcept_call(_fread_unlocked_r,__builtin_addressof(rent),begin,type_size,count,fp)
#else
	noexcept_call(_fread_r,__builtin_addressof(rent),begin,type_size,count,fp)
#endif
	};
	if(read_count==0)[[unlikely]]
	{
		if(rent._errno)
			throw_posix_error(rent._errno);
	}
#else
	std::size_t read_count{
#if defined(__CYGWIN__)
	my_cygwin_fread_unlocked(begin,type_size,count,fp)
#elif (defined(__USE_MISC) || defined(__BSD_VISIBLE)) && (!defined(__BIONIC__) || (defined(__USE_BSD) ))
#if !defined(fread_unlocked) && defined(__has_builtin)
#if __has_builtin(__builtin_fread_unlocked)
	__builtin_fread_unlocked(begin,type_size,count,fp)
#else
	fread_unlocked(begin,type_size,count,fp)
#endif
#else
	fread_unlocked(begin,type_size,count,fp)
#endif
#elif !defined(fread)&&defined(__has_builtin)
#if __has_builtin(__builtin_fread)
	__builtin_fread(begin,type_size,count,fp)
#else
	fread(begin,type_size,count,fp)
#endif
#else
	fread(begin,type_size,count,fp)
#endif
	};
	if(read_count==0)[[unlikely]]
	{
		if(
#if defined(__CYGWIN__)
	__sferror(fp)
#elif defined(__USE_MISC) || defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)
#if defined(__has_builtin)
#if __has_builtin(__builtin_ferror_unlocked)
	__builtin_ferror_unlocked(fp)
#else
	ferror_unlocked(fp)
#endif
#else
	ferror_unlocked(fp)
#endif
#elif !defined(ferror)&&defined(__has_builtin)
#if __has_builtin(__builtin_ferror)
	__builtin_ferror(fp)
#else
	ferror(fp)
#endif
#else
	ferror(fp)
#endif
		)
			throw_posix_error();
	}
#endif
	return read_count;
}

inline std::size_t c_fwrite_impl(void const* __restrict begin,std::size_t type_size,std::size_t count,FILE* __restrict fp)
{
	if(count==0)
		return 0;
#if defined(__NEWLIB__)&&!defined(__CYGWIN__)
	struct _reent rent{};
	std::size_t written_count{noexcept_call(_fwrite_r,__builtin_addressof(rent),begin,type_size,count,fp)};
	if(!written_count)[[unlikely]]
		throw_posix_error(rent._errno);
#else

	std::size_t written_count{
#if !defined(fwrite)&&defined(__has_builtin)
#if __has_builtin(__builtin_fwrite)
	__builtin_fwrite(begin,type_size,count,fp)
#else
	fwrite(begin,type_size,count,fp)
#endif
#else
	fwrite(begin,type_size,count,fp)
#endif
	};
	if(!written_count)[[unlikely]]
		throw_posix_error();
#endif
	return written_count;
}

inline std::size_t c_fread_impl(void* __restrict begin,std::size_t type_size,std::size_t count,FILE* __restrict fp)
{
	if(fp==stdin)
	{
		::fast_io::noexcept_call(::fflush,stdout);
	}
#if defined(__NEWLIB__)
	struct _reent rent{};
	std::size_t read_count{noexcept_call(_fread_r,__builtin_addressof(rent),begin,type_size,count,fp)};
	if(read_count==0)[[unlikely]]
	{
		if(rent._errno)
			throw_posix_error(rent._errno);
	}
#else
	std::size_t read_count{
#if !defined(fread)&&defined(__has_builtin)
#if __has_builtin(__builtin_fread)
	__builtin_fread(begin,type_size,count,fp)
#else
	fread(begin,type_size,count,fp)
#endif
#else
	fread(begin,type_size,count,fp)
#endif
	};
	if(read_count==0)[[unlikely]]
	{
		if(
#if !defined(ferror)&&defined(__has_builtin)
#if __has_builtin(__builtin_ferror)
	__builtin_ferror(fp)
#else
	ferror(fp)
#endif
#else
	ferror(fp)
#endif
		)
			throw_posix_error();
	}
#endif
	return read_count;
}

inline ::std::byte* c_read_some_bytes_impl(FILE* __restrict fp, ::std::byte* first, std::byte* last)
{
	return c_fread_impl(first,1,static_cast<::std::size_t>(last-first),fp)+first;
}

inline ::std::byte* c_unlocked_read_some_bytes_impl(FILE* __restrict fp, ::std::byte* first, std::byte* last)
{
	return c_fread_unlocked_impl(first,1,static_cast<::std::size_t>(last-first),fp)+first;
}

inline ::std::byte const* c_write_some_bytes_impl(FILE* __restrict fp, ::std::byte const* first, std::byte const* last)
{
	return c_fwrite_impl(first,1,static_cast<::std::size_t>(last-first),fp)+first;
}

inline ::std::byte const* c_unlocked_write_some_bytes_impl(FILE* __restrict fp, ::std::byte const* first, std::byte const* last)
{
	return c_fwrite_unlocked_impl(first,1,static_cast<::std::size_t>(last-first),fp)+first;
}

}

template<::fast_io::c_family family,::std::integral char_type>
inline ::std::byte* read_some_bytes_underflow_define(
::fast_io::basic_c_family_io_observer<family,char_type> ciob,
::std::byte *first,::std::byte *last)
{
	if constexpr(family==::fast_io::c_family::unlocked||
		family==::fast_io::c_family::emulated_unlocked)
	{
		return ::fast_io::details::c_unlocked_read_some_bytes_impl(ciob.fp,first,last);
	}
	else
	{
		return ::fast_io::details::c_read_some_bytes_impl(ciob.fp,first,last);
	}
}

template<::fast_io::c_family family,::std::integral char_type>
inline ::std::byte const* write_some_bytes_overflow_define(
::fast_io::basic_c_family_io_observer<family,char_type> ciob,
::std::byte const *first,::std::byte const *last)
{
	if constexpr(family==::fast_io::c_family::unlocked||
		family==::fast_io::c_family::emulated_unlocked)
	{
		return ::fast_io::details::c_unlocked_write_some_bytes_impl(ciob.fp,first,last);
	}
	else
	{
		return ::fast_io::details::c_write_some_bytes_impl(ciob.fp,first,last);
	}
}

}
