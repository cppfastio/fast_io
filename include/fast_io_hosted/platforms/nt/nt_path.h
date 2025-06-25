#pragma once

namespace fast_io::win32::nt::details
{
inline ::std::uint_least16_t nt_filename_bytes_check(::std::size_t bytes)
{
	constexpr ::std::size_t max_value{static_cast<::std::size_t>(::std::numeric_limits<::std::uint_least16_t>::max())};
	if (max_value < bytes)
	{
		throw_nt_error(0xC0000106);
	}
	return static_cast<::std::uint_least16_t>(bytes);
}

inline ::std::uint_least16_t strlen_to_nt_filename_bytes(::std::size_t str_sz)
{
	constexpr ::std::size_t max_value{static_cast<::std::size_t>(::std::numeric_limits<::std::uint_least16_t>::max() >> 1u)};
	if (max_value < str_sz)
	{
		throw_nt_error(0xC0000106);
	}
	return static_cast<::std::uint_least16_t>(str_sz << 1);
}

inline void nt_file_rtl_path(char16_t const *filename, win32::nt::unicode_string &nt_name, char16_t const *&part_name,
							 win32::nt::rtl_relative_name_u &relative_name)
{
/*
https://github.com/mirror/reactos/blob/master/reactos/dll/ntdll/def/ntdll.spec
ReactOS shows that RtlDosPathNameToNtPathName_U_WithStatus was added since Windows XP 64bit.
*/
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0502
	auto status{rtl_dos_path_name_to_nt_path_name_u_with_status(
		filename, __builtin_addressof(nt_name), __builtin_addressof(part_name), __builtin_addressof(relative_name))};
	if (status)
	{
		throw_nt_error(status);
	}
#else
	if (!win32::nt::rtl_dos_path_name_to_nt_path_name_u(
			filename, __builtin_addressof(nt_name), __builtin_addressof(part_name), __builtin_addressof(relative_name)))
	{
		throw_nt_error(0xC0000039);
	}
#endif
}

template <::std::integral char_type, typename func>
inline auto nt_call_invoke_with_directory_handle_impl(void *directory, char_type const *filename, ::std::size_t filename_len, func callback)
{
	using char16_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char16_t const *;
	if constexpr (::std::same_as<char_type, char16_t>)
	{
		using char16_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char16_t *;
		::std::uint_least16_t const bytes(strlen_to_nt_filename_bytes(filename_len));
		win32::nt::unicode_string relative_path{
			.Length = bytes, .MaximumLength = bytes, .Buffer = const_cast<char16_may_alias_ptr>(filename)};
		return callback(directory, __builtin_addressof(relative_path));
	}
	else if constexpr (sizeof(char_type) == sizeof(char16_t))
	{
		return nt_call_invoke_with_directory_handle_impl(
			directory, reinterpret_cast<char16_may_alias_const_ptr>(filename), filename_len, callback);
	}
	else
	{
		nt_api_encoding_converter converter(filename, filename_len);
		return nt_call_invoke_with_directory_handle_impl(
			directory, reinterpret_cast<char16_may_alias_const_ptr>(converter.c_str()), converter.size(), callback);
	}
}

inline void map_nt_unc_and_dos_path_to_nt_path(char16_t const *filename_c_str, win32::nt::unicode_string &us, win32::nt::rtl_unicode_string_unique_ptr &us_ptr)
{
	// UNC path, dos root path or relative path. You can use a left slash instead of a right slash
	char16_t const *part_name{};
	win32::nt::rtl_relative_name_u relative_name{};
	nt_file_rtl_path(filename_c_str, us, part_name, relative_name);
	us_ptr.heap_ptr = __builtin_addressof(us); // need free
}

template <::std::integral char_type, typename func>
	requires(sizeof(char_type) == sizeof(char16_t))
inline auto nt_call_invoke_without_directory_handle_impl(char_type const *filename_c_str, func callback)
{
	if constexpr (::std::same_as<char_type, char16_t>)
	{
		win32::nt::unicode_string nt_name;
		win32::nt::rtl_unicode_string_unique_ptr us_ptr{};
		map_nt_unc_and_dos_path_to_nt_path(filename_c_str, nt_name, us_ptr);
		return callback(nullptr, __builtin_addressof(nt_name));
	}
	else
	{
		using char16_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char16_t const *;
		return nt_call_invoke_without_directory_handle_impl(
			reinterpret_cast<char16_may_alias_const_ptr>(filename_c_str), callback);
	}
}

template <::std::integral char_type, typename func>
inline auto nt_call_invoke_without_directory_handle(char_type const *filename, ::std::size_t filename_len, func callback)
{
	using char16_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char16_t const *;
	if constexpr (sizeof(char_type) == sizeof(char16_t))
	{
		return nt_call_invoke_without_directory_handle_impl(reinterpret_cast<char16_may_alias_const_ptr>(filename), callback);
	}
	else
	{
		nt_api_encoding_converter converter(filename, filename_len);
		return nt_call_invoke_without_directory_handle_impl(
			reinterpret_cast<char16_may_alias_const_ptr>(converter.c_str()), callback);
	}
}

template <::std::integral char_type, typename func>
inline auto nt_call_callback(void *directory, char_type const *filename, ::std::size_t filename_len, func callback)
{
	if (directory == nullptr)
	{
		throw_nt_error(0xC0000008); // STATUS_INVALID_HANDLE
	}
	else if (directory == reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-3)))
	{
		return nt_call_invoke_without_directory_handle(filename, filename_len, callback);
	}
	return nt_call_invoke_with_directory_handle_impl(directory, filename, filename_len, callback);
}

template <::std::integral char_type, typename func>
inline auto nt_call_callback_without_directory_handle(char_type const *filename, ::std::size_t filename_len, func callback)
{
	return nt_call_invoke_without_directory_handle(filename, filename_len, callback);
}

template <typename func>
inline auto nt_call_kernel_common_impl(void *directory, char16_t const *filename, ::std::size_t filename_len, func callback)
{
	::std::uint_least16_t const bytes{strlen_to_nt_filename_bytes(filename_len)};
	win32::nt::unicode_string relative_path{
		.Length = bytes, .MaximumLength = bytes, .Buffer = const_cast<char16_t *>(filename)};
	return callback(directory, __builtin_addressof(relative_path));
}

template <typename func>
inline auto nt_call_kernel_nodir_callback(char16_t const *filename, ::std::size_t filename_len, func callback)
{
	return nt_call_kernel_common_impl(nullptr, filename, filename_len, callback);
}

template <typename func>
inline auto nt_call_kernel_callback(void *directory, char16_t const *filename, ::std::size_t filename_len,
									func callback)
{
	if (directory == nullptr)
	{
		throw_nt_error(0xC0000008); // STATUS_INVALID_HANDLE
	}
	else if (directory == reinterpret_cast<void *>(::std::ptrdiff_t(-3)))
	{
		directory = nullptr;
	}
	return nt_call_kernel_common_impl(directory, filename, filename_len, callback);
}

template <typename func>
inline auto nt_call_kernel_fs_dirent_callback(void *directory, char16_t const *filename, ::std::size_t filename_len,
											  func callback)
{
	if (directory == nullptr)
	{
		throw_nt_error(0xC0000008); // STATUS_INVALID_HANDLE
	}
	return nt_call_kernel_common_impl(directory, filename, filename_len, callback);
}

template <::std::integral char_type, typename func>
inline auto nt_call_determine_kernel_callback(void *directory, char_type const *filename, ::std::size_t filename_len, bool kernel, func callback)
{
	if (kernel)
	{
		return nt_call_kernel_callback(directory, filename, filename_len, callback);
	}
	else
	{
		return nt_call_callback(directory, filename, filename_len, callback);
	}
}

} // namespace fast_io::win32::nt::details
