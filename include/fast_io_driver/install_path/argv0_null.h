#pragma once

namespace fast_io::details
{
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
[[noreturn]]
inline ::fast_io::install_path get_module_install_path_from_argv0([[maybe_unused]] char const *argv0) 
{
	throw_win32_error(0);
}
} // namespace fast_io::details
