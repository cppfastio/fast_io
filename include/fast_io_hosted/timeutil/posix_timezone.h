#pragma once

namespace fast_io
{

struct posix_timezone_name
{
	char8_t *ptr{};
	std::size_t n{};
	constexpr posix_timezone_name() noexcept=default;
	constexpr posix_timezone_name(posix_timezone_name const&)=delete;
	posix_timezone_name& operator=(posix_timezone_name const&)=delete;
	constexpr posix_timezone_name(posix_timezone_name&& other) noexcept:ptr(other.ptr),n(other.n)
	{
		other.ptr=nullptr;
		other.n=0;
	}
	posix_timezone_name& operator=(posix_timezone_name&& other) noexcept
	{
		::fast_io::typed_generic_allocator_adapter<::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>,char8_t>::deallocate(this->ptr);
		this->ptr=other.ptr;
		this->n=other.n;
		other.ptr=nullptr;
		other.n=0;
		return *this;
	}
	constexpr
		~posix_timezone_name()
	{
		::fast_io::typed_generic_allocator_adapter<::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>,char8_t>::deallocate(ptr);
	}
};

namespace posix
{
extern int libc_lstat(char8_t const* pathname,struct stat* buf) noexcept __asm__("lstat");
extern std::ptrdiff_t libc_readlink(char8_t const* pathname,char8_t* buf, std::size_t bufsiz) noexcept __asm__("readlink");
}

namespace details
{

inline constexpr char8_t localtimezoneinfo[]{u8"/usr/share/zoneinfo/"};
inline constexpr std::size_t localtimezoneinfo_string_len{std::ranges::size(localtimezoneinfo)-1};

inline posix_timezone_name posix_localtimezone_impl() noexcept
{
	struct stat st;
	int ret{::fast_io::posix::libc_lstat(u8"/etc/localtime",__builtin_addressof(st))};
	if(ret==-1)
	{
		return {};
	}
	using unsignedtype = ::std::make_unsigned_t<decltype(st.st_size)>;
	if constexpr(sizeof(decltype(st.st_size))>sizeof(std::size_t))
	{
		constexpr std::size_t mx{::std::numeric_limits<std::size_t>::max()};
		if(mx<static_cast<unsignedtype>(st.st_size))
		{
			return {};
		}
	}
	std::size_t real_size{static_cast<std::size_t>(st.st_size)};
	char8_t* bufptr{::fast_io::typed_generic_allocator_adapter<::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>,char8_t>::allocate(real_size)};
	posix_timezone_name nm;
	nm.ptr=bufptr;
	nm.n=static_cast<std::size_t>(static_cast<unsignedtype>(st.st_size));
	std::ptrdiff_t symret{::fast_io::posix::libc_readlink(u8"/etc/localtime",bufptr,real_size)};
	if(ret==-1||static_cast<std::size_t>(symret)!=real_size||real_size<=localtimezoneinfo_string_len)
	{
		return {};
	}
	if(__builtin_memcmp(localtimezoneinfo,bufptr,localtimezoneinfo_string_len)!=0)
	{
		return {};
	}
	return nm;
}

}

struct posix_local_timezone
{
	inline posix_timezone_name operator()() noexcept
	{
		return ::fast_io::details::posix_localtimezone_impl();
	}
};

inline constexpr cross_code_cvt_t<char8_t> print_alias_define(::fast_io::io_alias_t,posix_timezone_name const& enm) noexcept
{
	if(enm.ptr==nullptr)
	{
		return ::fast_io::cross_code_cvt_t<char8_t>{{u8"Etc/GMT",7u}};
	}
	else
	{
		return ::fast_io::cross_code_cvt_t<char8_t>{{enm.ptr+::fast_io::details::localtimezoneinfo_string_len,enm.n-::fast_io::details::localtimezoneinfo_string_len}};
	}
}

#ifndef __CYGWIN__
using native_local_timezone = posix_local_timezone;
#endif
}
