#pragma once

#include <stdlib.h>

namespace fast_io::details
{
/*
 * for win32_9xa
 */
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path()
{
	char *pgmptr{_pgmptr};

	if (pgmptr == nullptr)[[unlikely]]
	{
		throw_win32_error(24);
	}
	
	/* Will be affected by regional encoding settings, and _get_wpgmptr may not succeed, so only use on windows 9x */
	::fast_io::install_path ret;
	ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(pgmptr));
	auto const begin{strlike_begin(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
	auto curr{strlike_curr(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
	for (; curr != begin; --curr) // calculate dos path
	{
		if (auto const c{*curr}; c == u8'\\' || c == u8':'  || c == u8'/')
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
