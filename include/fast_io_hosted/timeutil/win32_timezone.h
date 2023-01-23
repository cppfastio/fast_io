#pragma once

namespace fast_io
{

struct win32_timezone_name
{
	std::size_t position{183};
};

enum win32_timezone_family
{
registry,
gettimezoneinformation,
#ifdef __BIONIC__
//Intel pintool does not support registry api
native = gettimezoneinformation
#else
native = registry
#endif
};

namespace win32::details
{
#include"win32_regtz_to_ianatz.h"

struct win32_registry_guard
{
	::std::uintptr_t k;
	explicit constexpr win32_registry_guard(::std::uintptr_t k1) noexcept:k(k1){}
	win32_registry_guard(win32_registry_guard const&)=delete;
	win32_registry_guard& operator=(win32_registry_guard const&)=delete;
	~win32_registry_guard()
	{
		::fast_io::win32::RegCloseKey(k);
	}
};

template<typename T>
requires (::std::same_as<T,char8_t>||::std::same_as<T,char16_t>)
inline constexpr win32_timezone_name find_win32_regtz_position_impl(T const* str,std::size_t len) noexcept
{
	::fast_io::basic_io_scatter_t<T> scatter{str,len};
	::fast_io::basic_io_scatter_t<T> const* first;
	::fast_io::basic_io_scatter_t<T> const* last;
	if constexpr(::std::same_as<T,char16_t>)
	{
		first = win32_registry_timezone_ntw;
		last = win32_registry_timezone_ntw+::std::ranges::size(win32_registry_timezone_ntw);
	}
	else
	{
		first = win32_registry_timezone_9xa;
		last = win32_registry_timezone_9xa+::std::ranges::size(win32_registry_timezone_9xa);
	}
	auto it{::fast_io::freestanding::lower_bound(first,last,
		scatter,
		[](::fast_io::basic_io_scatter_t<T> const& a,::fast_io::basic_io_scatter_t<T> const& b)
	{
		return ::fast_io::freestanding::lexicographical_compare(a.base,a.base+a.len,b.base,b.base+b.len);
	})};
	if(it==last)
	{
		return {};
	}
	if(!::fast_io::freestanding::equal(it->base,it->base+it->len,str,str+len))
	{
		return {};
	}
	return {static_cast<std::size_t>(it-first)};
}

template<win32_timezone_family tzfamily,win32_family family>
inline win32_timezone_name win32_localtimezone_impl() noexcept
{
	using api_char_type = std::conditional_t<family==win32_family::ansi_9x,char8_t,char16_t>;
	if constexpr(tzfamily == win32_timezone_family::gettimezoneinformation)
	{
		static_assert(tzfamily != win32_timezone_family::gettimezoneinformation
			|| family!=win32_family::ansi_9x);
		::fast_io::win32::time_zone_information tzi{};
		::std::uint_least32_t retval{::fast_io::win32::GetTimeZoneInformation(__builtin_addressof(tzi))};
		if(retval==UINT_LEAST32_MAX)
		{
			throw_win32_error();
		}
		std::size_t const standardnamesize{::fast_io::cstr_nlen(tzi.StandardName,sizeof(tzi.StandardName))};
		return ::fast_io::win32::details::find_win32_regtz_position_impl(tzi.StandardName,standardnamesize);
	}
	else
	{
		::std::uintptr_t k;
		::std::int_least32_t win32retcode;
		if constexpr(family==win32_family::ansi_9x)
		{
			win32retcode=::fast_io::win32::RegOpenKeyA(0x80000002 /*HKEY_LOCAL_MACHINE*/,u8"SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation",__builtin_addressof(k));
		}
		else
		{
			win32retcode=::fast_io::win32::RegOpenKeyW(0x80000002 /*HKEY_LOCAL_MACHINE*/,u"SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation",__builtin_addressof(k));
		}
		if(win32retcode)
		{
			return {};
		}
		constexpr
			::std::size_t n{128};
		api_char_type buffer[n];
		constexpr std::uint_least32_t bfbytes{n*sizeof(api_char_type)};
		::std::uint_least32_t cbdata{bfbytes};
		{
			::fast_io::win32::details::win32_registry_guard guard(k);
			if constexpr(family==win32_family::ansi_9x)
			{
				win32retcode=::fast_io::win32::RegQueryValueExA(k,u8"TimeZoneKeyName",nullptr,nullptr,buffer,__builtin_addressof(cbdata));
			}
			else
			{
				win32retcode=::fast_io::win32::RegQueryValueExW(k,u"TimeZoneKeyName",nullptr,nullptr,buffer,__builtin_addressof(cbdata));
			}
			if(win32retcode)
			{
				cbdata=bfbytes;
				if constexpr(family==win32_family::ansi_9x)
				{
					win32retcode=::fast_io::win32::RegQueryValueExA(k,u8"StandardName",nullptr,nullptr,buffer,__builtin_addressof(cbdata));
				}
				else
				{
					win32retcode=::fast_io::win32::RegQueryValueExW(k,u"StandardName",nullptr,nullptr,buffer,__builtin_addressof(cbdata));
				}
				if(win32retcode)
				{
					return {};
				}
			}
		}
		if constexpr(sizeof(api_char_type)!=1u)
		{
			cbdata/=sizeof(api_char_type);
		}
		if(!cbdata)
		{
			return {};
		}
		std::size_t const stringlen{static_cast<std::size_t>(cbdata-1u)};
		return ::fast_io::win32::details::find_win32_regtz_position_impl(buffer,stringlen);
	}
}

}

template<win32_timezone_family tzfamily,win32_family family>
struct basic_win32_family_local_timezone
{
	inline win32_timezone_name operator()() noexcept
	{
		return ::fast_io::win32::details::win32_localtimezone_impl<tzfamily,family>();
	}
};

template<win32_family family>
using basic_win32_local_timezone = basic_win32_family_local_timezone<win32_timezone_family::native,family>;
using win32_local_timezone_9xa = basic_win32_local_timezone<::fast_io::win32_family::ansi_9x>;
using win32_local_timezone_ntw = basic_win32_local_timezone<::fast_io::win32_family::wide_nt>;
using win32_local_timezone = basic_win32_local_timezone<::fast_io::win32_family::native>;

#ifndef __CYGWIN__
using native_local_timezone = win32_local_timezone;
#endif

template<std::integral char_type>
inline constexpr basic_io_scatter_t<char_type> status_io_print_forward(io_alias_type_t<char_type>,win32_timezone_name tzn) noexcept
{
	if constexpr(::std::same_as<char_type,char>)
		return ::fast_io::win32::details::iana_db_timezone[tzn.position];
	else if constexpr(::std::same_as<char_type,wchar_t>)
		return ::fast_io::win32::details::wiana_db_timezone[tzn.position];
	else if constexpr(::std::same_as<char_type,char16_t>)
		return ::fast_io::win32::details::u16iana_db_timezone[tzn.position];
	else if constexpr(::std::same_as<char_type,char32_t>)
		return ::fast_io::win32::details::u32iana_db_timezone[tzn.position];
	else
		return ::fast_io::win32::details::u8iana_db_timezone[tzn.position];
}

}