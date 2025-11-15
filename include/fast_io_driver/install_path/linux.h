#pragma once

#if defined(__linux__)
#include <linux/limits.h>
#else
#include <limits.h>
#endif

namespace fast_io::details
{
/*
 * for linux, cygwin and solaris
 */
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path()
{
	constexpr ::std::size_t path_max{::std::max<::std::size_t>(PATH_MAX, 4096u)};

	char buffer[path_max + 1];
	::fast_io::install_path ret;

	using my_ssize_t [[maybe_unused]] = ::std::make_signed_t<::std::size_t>;
#if defined(__linux__) && defined(__NR_readlink)
	auto resolved{::fast_io::system_call<__NR_readlink, my_ssize_t>("/proc/self/exe", buffer, path_max)};
	system_call_throw_error(resolved);
	if (static_cast<::std::size_t>(resolved) >= path_max)
	{
		throw_posix_error(ERANGE);
	}
	buffer[resolved] = '\0';
	ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt(::fast_io::mnp::os_c_str_with_known_size(buffer, resolved)));
#else
	char *resolved{
		::fast_io::noexcept_call(::realpath,
#if defined(__sun)
								 "/proc/self/path/a.out"
#else
								 "/proc/self/exe"
#endif
								 ,
								 buffer)};
	if (!resolved) [[unlikely]]
	{
		throw_posix_error();
	}
	ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(resolved));
#endif
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
