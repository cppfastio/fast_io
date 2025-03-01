#pragma once
namespace fast_io::details
{
template <::std::integral char_type>
inline constexpr bool is_invalid_dos_filename_with_size(char_type const *filename, ::std::size_t filename_size) noexcept
{
	auto const os_str_filename_len{filename_size};

	if (os_str_filename_len == 0) [[unlikely]]
	{
		return true;
	}

	if (os_str_filename_len == 1 &&
		::fast_io::char_category::is_dos_path_invalid_prefix_character(filename[0])) [[unlikely]]
	{
		return true;
	}

	if (os_str_filename_len == 2 &&
		::fast_io::char_category::is_dos_path_invalid_prefix_character(filename[0]) &&
		::fast_io::char_category::is_dos_path_invalid_prefix_character(filename[1])) [[unlikely]]
	{
		return true;
	}

	auto curr{filename};
	auto const end{filename + os_str_filename_len};

	for (; curr != end; ++curr)
	{
		if (auto fc{*curr}; ::fast_io::char_category::is_dos_path_invalid_character(fc)) [[unlikely]]
		{
			return true;
		}
	}

	return false;
}

template <::std::integral char_type>
inline constexpr bool is_invalid_dos_filename(char_type const *filename) noexcept
{
	return is_invalid_dos_filename_with_size(filename, ::fast_io::cstr_len(filename));
}

} // namespace fast_io::details
