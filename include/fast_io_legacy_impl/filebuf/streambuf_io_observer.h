#pragma once

#include"fp_hack/impl.h"

namespace fast_io
{

template<typename T>
#if !defined(_MSC_VER) || defined(__clang__)
requires std::derived_from<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>
#endif
class basic_general_streambuf_io_observer
{
public:
	using streambuf_type = T;
	using char_type = typename streambuf_type::char_type;
	using traits_type = typename streambuf_type::traits_type;
	using native_handle_type = streambuf_type*;
	native_handle_type fb{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return fb;
	}
	inline constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{fb};
		fb=nullptr;
		return temp;
	}
	explicit constexpr operator bool() const noexcept
	{
		return fb;
	}
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_MSVC_STL_UPDATE)
	template<c_family family>
	explicit operator basic_c_family_io_observer<family,char_type>() const noexcept
	{
		return basic_c_io_observer<char_type>{details::streambuf_hack::fp_hack(fb)};
	}
#if !defined(__AVR__)
	explicit operator basic_posix_io_observer<char_type>() const noexcept
	{
		return static_cast<basic_posix_io_observer<char_type>>(static_cast<basic_c_io_observer<char_type>>(*this));
	}
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
	explicit operator basic_win32_io_observer<char_type>() const noexcept
	{
		return static_cast<basic_win32_io_observer<char_type>>
		(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
	template<nt_family fam>
	explicit operator basic_nt_family_io_observer<fam,char_type>() const noexcept
	{
		return static_cast<basic_nt_family_io_observer<fam,char_type>>
		(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
#endif
#endif
#endif
};

template<typename T>
inline void flush(basic_general_streambuf_io_observer<T> h)
{
	if(h.fb->pubsync()==-1)
		throw_posix_error(EIO);
}

#if __cpp_lib_three_way_comparison >= 201907L

template<typename T>
inline constexpr bool operator==(basic_general_streambuf_io_observer<T> a,basic_general_streambuf_io_observer<T> b) noexcept
{
	return a.fb==b.fb;
}

template<typename T>
inline constexpr auto operator<=>(basic_general_streambuf_io_observer<T> a,basic_general_streambuf_io_observer<T> b) noexcept
{
	return a.fb<=>b.fb;
}

#endif

template<typename T>
inline constexpr basic_general_streambuf_io_observer<T> io_value_handle(basic_general_streambuf_io_observer<T> other) noexcept
{
	return other;
}

template<std::integral CharT,typename Traits = std::char_traits<CharT>>
using basic_streambuf_io_observer = basic_general_streambuf_io_observer<std::basic_streambuf<CharT,Traits>>;

template<std::integral CharT,typename Traits = std::char_traits<CharT>>
using basic_filebuf_io_observer = basic_general_streambuf_io_observer<std::basic_filebuf<CharT,Traits>>;

using streambuf_io_observer = basic_streambuf_io_observer<char>;
using filebuf_io_observer = basic_filebuf_io_observer<char>;

using wstreambuf_io_observer = basic_streambuf_io_observer<wchar_t>;
using wfilebuf_io_observer = basic_filebuf_io_observer<wchar_t>;
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_MSVC_STL_UPDATE)
template<std::integral ch_type,typename Traits>
requires zero_copy_input_stream<basic_c_io_observer<ch_type>>
inline constexpr decltype(auto) zero_copy_in_handle(basic_filebuf_io_observer<ch_type,Traits> h)
{
	return zero_copy_in_handle(static_cast<basic_c_io_observer<ch_type>>(h));
}
template<std::integral ch_type,typename Traits>
requires zero_copy_output_stream<basic_c_io_observer<ch_type>>
inline constexpr decltype(auto) zero_copy_out_handle(basic_filebuf_io_observer<ch_type,Traits> h)
{
	return zero_copy_out_handle(static_cast<basic_c_io_observer<ch_type>>(h));
}

template<std::integral ch_type,typename Traits>
inline auto seek(basic_filebuf_io_observer<ch_type,Traits> h,std::intmax_t offset=0,seekdir s=seekdir::cur)
{
	h.fb->pubsync();
	return seek(static_cast<basic_c_io_observer<ch_type>>(h),offset,s);
}

template<std::integral ch_type,typename traits_type,typename... Args>
requires io_controllable<basic_c_io_observer<ch_type>,Args...>
inline decltype(auto) io_control(basic_filebuf_io_observer<ch_type,traits_type> h,Args&& ...args)
{
	return io_control(static_cast<basic_c_io_observer<ch_type>>(h),::std::forward<Args>(args)...);
}

template<std::integral ch_type,typename traits_type>
requires requires(basic_c_io_observer<ch_type> piob)
{
	status(piob);
}
inline constexpr auto status(basic_streambuf_io_observer<ch_type,traits_type> ciob)
{
	return status(static_cast<basic_c_io_observer<ch_type>>(ciob));
}

template<std::integral ch_type,typename traits_type>
requires requires(basic_c_io_observer<ch_type> piob)
{
	status(piob);
}
inline constexpr auto status(basic_filebuf_io_observer<ch_type,traits_type> ciob)
{
	return status(static_cast<basic_c_io_observer<ch_type>>(ciob));
}

template<std::integral char_type,typename traits_type>
inline constexpr posix_at_entry at(basic_filebuf_io_observer<char_type,traits_type> other) noexcept
{
	return posix_at_entry{details::fp_to_fd(details::streambuf_hack::fp_hack(other.fb))};
}

template<typename T>
requires (std::same_as<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>||
std::derived_from<T,std::basic_filebuf<typename T::char_type,typename T::traits_type>>)
inline bool is_character_device(basic_general_streambuf_io_observer<T> other)
{
	using char_type  = typename T::char_type;
	basic_c_io_observer<char_type> bciob{static_cast<basic_c_io_observer<char_type>>(other)};
	if constexpr(std::same_as<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>)
	{
		if(bciob.fp==nullptr)
#ifdef EBADF
			throw_posix_error(EBADF);
#else
			throw_posix_error(EINVAL);
#endif
	}
	return is_character_device(bciob);

}

template<typename T>
requires (std::same_as<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>||
std::derived_from<T,std::basic_filebuf<typename T::char_type,typename T::traits_type>>)
inline void clear_screen(basic_general_streambuf_io_observer<T> other)
{
	using char_type  = typename T::char_type;
	basic_c_io_observer<char_type> bciob{static_cast<basic_c_io_observer<char_type>>(other)};
	if constexpr(std::same_as<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>)
	{
		if(bciob.fp==nullptr)
#ifdef EBADF
			throw_posix_error(EBADF);
#else
			throw_posix_error(EINVAL);
#endif
	}
#if defined(__AVR__)
	::fast_io::details::avr_libc_nosup_impl();
#else
	io_lock_guard guard{bciob};
	FILE* fp{bciob.fp};
	int fd{details::fp_unlocked_to_fd(fp)};
#if defined(_WIN32)&&!defined(__WINE__)
	void* handle{details::my_get_osfile_handle(fd)};
	if(!win32::details::win32_is_character_device(handle))
		return;
	other.fb->pubsync();
	details::c_flush_unlocked_impl(fp);
	win32::details::win32_clear_screen_main(handle);
#else
	if(!details::posix_is_character_device(fd))
		return;
	other.fb->pubsync();
	details::c_flush_unlocked_impl(fp);
	details::posix_clear_screen_main(fd);
#endif
#endif
}
#endif
}

#include"bp_hack/impl.h"

namespace fast_io
{

namespace details
{

template<std::integral char_type,typename traits_type>
inline std::size_t streambuf_write_report_eh_impl(std::basic_streambuf<char_type,traits_type>* fb,
	char_type const* src, std::size_t count)
{
	return static_cast<std::size_t>(fb->sputn(src,static_cast<std::streamsize>(count)));
}

template<std::integral char_type,typename traits_type>
inline std::size_t streambuf_read_report_eh_impl(std::basic_streambuf<char_type,traits_type>* fb,char_type* dest, std::size_t count)
{
	return static_cast<std::size_t>(fb->sgetn(dest,static_cast<std::streamsize>(count)));
}


template<std::integral char_type,typename traits_type>
inline std::size_t streambuf_write_impl(basic_streambuf_io_observer<char_type,traits_type> t,char_type const* src, std::size_t count)
{
	auto curr{obuffer_curr(t)};
	auto ed{obuffer_end(t)};
	if(count<static_cast<std::size_t>(ed-curr))[[likely]]
	{
		traits_type::copy(curr,src,count);
		obuffer_set_curr(t,curr+count);
		return count;
	}
	return streambuf_write_report_eh_impl(t.fb,src,count);
}

template<std::integral char_type,typename traits_type>
inline std::size_t streambuf_read_impl(basic_streambuf_io_observer<char_type,traits_type> t,char_type* dest, std::size_t count)
{
	auto curr{ibuffer_curr(t)};
	auto ed{ibuffer_end(t)};
	if(count<static_cast<std::size_t>(ed-curr))[[likely]]
	{
		traits_type::copy(dest,curr,count);
		ibuffer_set_curr(t,curr+count);
		return count;
	}
	return streambuf_read_report_eh_impl(t.fb,dest,count);
}


}

template<typename T,::std::contiguous_iterator Iter>
requires (std::same_as<typename T::char_type,::std::iter_value_t<Iter>>||std::same_as<typename T::char_type,char>)
[[nodiscard]] inline Iter read(basic_general_streambuf_io_observer<T> t,Iter begin,Iter end)
{
	using char_type = typename T::char_type;
	using traits_type = typename T::traits_type;
	if constexpr(std::same_as<typename T::char_type,::std::iter_value_t<Iter>>)
		return begin+details::streambuf_read_impl(basic_streambuf_io_observer<char_type,traits_type>{t.fb},
			::std::to_address(begin),static_cast<std::size_t>(end-begin));
	else
		return begin+details::streambuf_read_impl(basic_streambuf_io_observer<char_type,traits_type>{t.fb},
			reinterpret_cast<char*>(::std::to_address(begin)),static_cast<std::size_t>(end-begin)*sizeof(*begin))/(*begin);
}

template<typename T,::std::contiguous_iterator Iter>
requires (std::same_as<typename T::char_type,::std::iter_value_t<Iter>>||std::same_as<typename T::char_type,char>)
inline Iter write(basic_general_streambuf_io_observer<T> t,Iter begin,Iter end)
{
	using char_type = typename T::char_type;
	using traits_type = typename T::traits_type;
	if constexpr(std::same_as<typename T::char_type,::std::iter_value_t<Iter>>)
		return begin+details::streambuf_write_impl(basic_streambuf_io_observer<char_type,traits_type>{t.fb},
			::std::to_address(begin),static_cast<std::size_t>(end-begin));
	else
		return begin+details::streambuf_write_impl(basic_streambuf_io_observer<char_type,traits_type>{t.fb},
			reinterpret_cast<char const*>(::std::to_address(begin)),static_cast<std::size_t>(end-begin)*sizeof(*begin))/(*begin);
}
//static_assert(value_based_stream<filebuf_io_observer>);
}
