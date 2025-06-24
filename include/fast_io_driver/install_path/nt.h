#pragma once

namespace fast_io::details
{
/*
 * for Windows NT
 */
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline ::fast_io::install_path get_module_install_path()
{
	auto c_peb{::fast_io::win32::nt::nt_get_current_peb()};
	auto const &NtImagePath{c_peb->ProcessParameters->ImagePathName};
	if (!NtImagePath.Buffer) [[unlikely]]
	{
		::fast_io::throw_nt_error(0xC0000106);
	}

	::fast_io::install_path ret;

	ret.module_name = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt(::fast_io::mnp::os_c_str_with_known_size(NtImagePath.Buffer, NtImagePath.Length / sizeof(char16_t))));
	auto const begin{strlike_begin(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
	auto curr{strlike_curr(::fast_io::io_strlike_type<char8_t, ::fast_io::u8string>, ret.module_name)};
	for (; curr != begin; --curr) // calculate nt, dos or (nt) device path
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
