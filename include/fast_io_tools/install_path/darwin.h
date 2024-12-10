#pragma once

#include <mach-o/dyld.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdbool.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

namespace fast_io::details
{
/*
 * unknown-apple-darwin
 * https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
 * for Mac OS, IOS, watch os, ...
 */
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path()
{
	char buffer[PATH_MAX + 1];
	::std::uint_least32_t size{PATH_MAX};
	if (::fast_io::noexcept_call(::_NSGetExecutablePath, buffer, __builtin_addressof(size)) == -1) [[unlikely]]
	{
		throw_posix_error();
	}
	char buffer2[PATH_MAX + 1];
	char *resolved{::fast_io::noexcept_call(::realpath, buffer, buffer2)};
	if (!resolved) [[unlikely]]
	{
		throw_posix_error();
	}

	::fast_io::install_path ret;

	ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(resolved));
	auto const begin{strlike_begin(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
	auto curr{strlike_curr(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
	for (; curr != begin; --curr)
	{
		if (auto const c{*curr}; c == u8'/')
		{
			++curr;
			break;
		}
	}
	ret.path_name = ::fast_io::u8string{begin, curr - 1};
	ret.module_name.erase(begin, curr);
	return ret;
}
} // namespace fast_io::details
