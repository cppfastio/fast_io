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

inline std::size_t c_read_impl(void* __restrict begin,std::size_t type_size,std::size_t count,FILE* __restrict fp)
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

template<c_family family,std::integral char_type>
inline std::size_t c_io_write_impl(basic_c_family_io_observer<family,char_type> cfhd,char_type const* begin,char_type const* end)
{
	std::ptrdiff_t const count(end-begin);
	if constexpr(family==c_family::unlocked)
	{
		if constexpr(buffer_output_stream_impl<basic_c_family_io_observer<family,char_type>>)
		{
			auto curr{obuffer_curr(cfhd)};
			auto ed{obuffer_end(cfhd)};
			if(count<ed-curr)[[likely]]
			{
				non_overlapped_copy_n(begin,static_cast<std::size_t>(count),curr);
				obuffer_set_curr(cfhd,curr+count);
				return static_cast<std::size_t>(count);
			}
		}
		return c_fwrite_unlocked_impl(begin,sizeof(char_type),static_cast<std::size_t>(count),cfhd.fp);
	}
	else if constexpr(buffer_output_stream_impl<basic_c_family_io_observer<c_family::unlocked,char_type>>)
	{
		io_lock_guard guard{cfhd};
		return c_io_write_impl(basic_c_io_observer_unlocked<char_type>{cfhd.fp},begin,end);
	}
	else
		return c_fwrite_impl(begin,sizeof(char_type),static_cast<std::size_t>(count),cfhd.fp);
}

template<c_family family,std::integral char_type>
inline std::size_t c_io_read_impl(basic_c_family_io_observer<family,char_type> cfhd,char_type* begin,char_type* end)
{
	std::ptrdiff_t const count(end-begin);
	if constexpr(buffer_input_stream_impl<basic_c_family_io_observer<family,char_type>>)
	{
		auto curr{ibuffer_curr(cfhd)};
		auto ed{ibuffer_end(cfhd)};
		if(count<ed-curr)[[likely]]
		{
			non_overlapped_copy_n(curr,static_cast<std::size_t>(count),begin);
			ibuffer_set_curr(cfhd,curr+count);
			return static_cast<std::size_t>(count);
		}
		return c_fread_unlocked_impl(begin,sizeof(char_type),static_cast<std::size_t>(count),cfhd.fp);
	}
	else if constexpr(buffer_input_stream_impl<basic_c_family_io_observer<c_family::unlocked,char_type>>)
	{
		io_lock_guard guard{cfhd};
		return c_io_read_impl(basic_c_io_observer_unlocked<char_type>{cfhd.fp},begin,end);
	}
	else
		return c_read_impl(begin,sizeof(char_type),static_cast<std::size_t>(count),cfhd.fp);
}
#if 0
template<c_family family,std::integral char_type>
inline io_scatter_status_t c_io_scatter_write_impl(basic_c_family_io_observer<family,char_type> cfhd,basic_io_scatter_t<char_type> const* scatters,std::size_t n)
{
	if constexpr(family==c_family::standard)
	{
		if(n==0)
			return {0,0,0};
		if constexpr(buffer_output_stream_impl<basic_c_family_io_observer<family,char_type>>)
		{
			lock_guard guard{cfhd};
			return c_io_scatter_write_impl(basic_c_io_observer_unlocked<char_type>{cfhd.fp},begin,end);

		}
		else
		{
			if(n==1)
				return c_fwrite_impl(scatters[0].base,,scatters[0].len);
			lock_guard guard{cfhd};
			return c_io_scatter_write_impl(basic_c_io_observer_unlocked<char_type>{cfhd.fp},begin,end);
		}
	}
	else
	{
		std::size_t total_written{};
		for(std::size_t i{};i!=n;++i)
		{
			io_scatter_t scat{scatters[i]};
			std::size_t sz{c_io_write_impl(cfhd,scat.base,scat.len)};
			total_written+=sz;
			if(sz!=scat.len)
				return {total_written,i,sz};
		}
		return {total_written,n,0};
	}
}

template<c_family family>
inline io_scatter_status_t c_io_scatter_read_impl(basic_c_family_io_observer<family,char_type> cfhd,io_scatter_t const* scatters,std::size_t n)
{

}
#endif
}

template<c_family family,std::integral T,::std::contiguous_iterator Iter>
requires (std::same_as<T,::std::iter_value_t<Iter>>||std::same_as<T,char>)
inline Iter write(basic_c_family_io_observer<family,T> cfhd,Iter cbegin,Iter cend)
{
	if constexpr(std::same_as<::std::iter_value_t<Iter>,T>)
		return cbegin+details::c_io_write_impl(cfhd,::std::to_address(cbegin),::std::to_address(cend));
	else
		return cbegin+details::c_io_write_impl(cfhd,reinterpret_cast<char const*>(::std::to_address(cbegin)),reinterpret_cast<char const*>(::std::to_address(cend)))/sizeof(*cbegin);
}

template<c_family family,std::integral T,::std::contiguous_iterator Iter>
requires (std::same_as<T,::std::iter_value_t<Iter>>||std::same_as<T,char>)
[[nodiscard]] inline Iter read(basic_c_family_io_observer<family,T> cfhd,Iter begin,Iter end)
{
	if constexpr(std::same_as<::std::iter_value_t<Iter>,T>)
		return begin+details::c_io_read_impl(cfhd,::std::to_address(begin),::std::to_address(end));
	else
		return begin+details::c_io_read_impl(cfhd,reinterpret_cast<char*>(::std::to_address(begin)),reinterpret_cast<char*>(::std::to_address(end)))/sizeof(*begin);
}

}
