#pragma once

#include <limits.h>
#include <stdlib.h>
#include <cstring>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

namespace fast_io::details
{
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path_from_argv0(char const *argv0)
{
	if (!argv0) [[unlikely]]
	{
		throw_posix_error();
	}

	::fast_io::install_path ret;

	char newpath[PATH_MAX + 256 + 1];
	char newpath2[PATH_MAX + 256 + 1];

	newpath[PATH_MAX + 256] = '0';
	newpath2[PATH_MAX + 256] = '0';

	constexpr char path_separator = '/';
	char path_separator_as_string[2] = "/";
	char path_list_separator[8] = ":"; // could be ":; "
	if (argv0[0] == path_separator)
	{
		[[maybe_unused]] auto const unused1{::fast_io::noexcept_call(realpath, argv0, newpath)};
		if (auto status{::fast_io::noexcept_call(access, newpath, F_OK)}; !status)
		{
			newpath[PATH_MAX - 1] = 0;
			ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(newpath));
			auto const begin{strlike_begin(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
			auto curr{strlike_curr(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
			for (; curr != begin; --curr) // calculate dos path
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
		else
		{
			throw_posix_error(status);
		}
	}
	else if (__builtin_strchr(argv0, static_cast<int>(path_separator)))
	{
		[[maybe_unused]] auto const unused1{::fast_io::noexcept_call(getcwd, newpath2, PATH_MAX)};
		::fast_io::noexcept_call(strncat, newpath2, path_separator_as_string, PATH_MAX + 256);
		::fast_io::noexcept_call(strncat, newpath2, argv0, PATH_MAX + 256);
		[[maybe_unused]] auto const unused2{::fast_io::noexcept_call(realpath, newpath2, newpath)};
		if (auto status{::fast_io::noexcept_call(access, newpath, F_OK)};!status)
		{
			newpath[PATH_MAX - 1] = 0;
			ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(newpath));
			auto const begin{strlike_begin(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
			auto curr{strlike_curr(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
			for (; curr != begin; --curr) // calculate dos path
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
		else
		{
			throw_posix_error(status);
		}
	}
	else
	{
		char *saveptr;
		char *pathitem;
		char *save_path{::fast_io::noexcept_call(getenv, "PATH")};
		for (pathitem = ::fast_io::noexcept_call(strtok_r, save_path, path_list_separator, &saveptr); pathitem;
			 pathitem = ::fast_io::noexcept_call(strtok_r, nullptr, path_list_separator, &saveptr))
		{
			::fast_io::noexcept_call(strncpy, newpath2, pathitem, PATH_MAX + 256);
			::fast_io::noexcept_call(strncat, newpath2, path_separator_as_string, PATH_MAX + 256);
			::fast_io::noexcept_call(strncat, newpath2, argv0, PATH_MAX + 256);
			[[maybe_unused]] auto const unused1{::realpath(newpath2, newpath)};
			if (!::fast_io::noexcept_call(access, newpath, F_OK))
			{
				newpath[PATH_MAX - 1] = 0;
				ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(newpath));
				auto const begin{strlike_begin(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
				auto curr{strlike_curr(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
				for (; curr != begin; --curr) // calculate dos path
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
		} // end for
		throw_posix_error();

	} // end else
}
} // namespace fast_io::details
