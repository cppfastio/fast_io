#pragma once
namespace fast_io::details
{
template <::std::integral char_type>
inline constexpr bool is_valid_os_file_name(char_type const *filename, ::std::size_t filename_size) noexcept
{
	auto const os_str_filename_len{::fast_io::cstr_nlen(filename, filename_size)};
	if (os_str_filename_len == 0) [[unlikely]]
	{
		return false;
	}
	constexpr char_type pointer_char_type{::fast_io::char_literal_v<u8'.', char_type>};
	if (os_str_filename_len == 1 && filename[0] == pointer_char_type) [[unlikely]]
	{
		return false;
	}
	if (os_str_filename_len == 2 && filename[0] == pointer_char_type && filename[1] == pointer_char_type)
	{
		return false;
	}
	auto curr{filename};
	auto const end{filename + os_str_filename_len};
	for (; curr != end; ++curr)
	{
		auto fc{*curr};
		if (static_cast<::std::make_unsigned_t<char_type>>(fc) < 32u ||
			fc == ::fast_io::char_literal_v<u8'/', char_type> ||
			fc == ::fast_io::char_literal_v<u8'\\', char_type> ||
			fc == ::fast_io::char_literal_v<u8':', char_type> ||
			fc == ::fast_io::char_literal_v<u8'*', char_type> ||
			fc == ::fast_io::char_literal_v<u8'?', char_type> ||
			fc == ::fast_io::char_literal_v<u8'\"', char_type> ||
			fc == ::fast_io::char_literal_v<u8'<', char_type> ||
			fc == ::fast_io::char_literal_v<u8'>', char_type> ||
			fc == ::fast_io::char_literal_v<u8'|', char_type>) [[unlikely]]
		{
			return false;
		}
	}
	return true;
}
} // namespace fast_io::details
