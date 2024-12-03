#pragma once

namespace fast_io::details
{
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
[[noreturn]]
inline ::fast_io::install_path get_module_install_path_from_argv0(char const *argv0) 
{
	throw_win32_error(0);
}
} // namespace uwvm::path
