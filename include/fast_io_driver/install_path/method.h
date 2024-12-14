#pragma once

namespace fast_io
{
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path()
{
	return ::fast_io::details::get_module_install_path();
}

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path_from_argv0(char const *argv0)
{
	return ::fast_io::details::get_module_install_path_from_argv0(argv0);
}
}
