#pragma once

namespace fast_io
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	path_dir_file_path
{
	::std::size_t length{};
};

template <typename allocator>
class basic_path_dir_file
{
public:
	char8_t const *directory_path_buffer{};
	::std::size_t directory_path_length{};

	inline explicit constexpr basic_path_dir_file() noexcept

		basic_path_dir_file(basic_path_dir_file const &) = delete;
	inline basic_path_dir_file &operator=(basic_path_dir_file const &) = delete;

	inline constexpr basic_path_dir_file(basic_path_dir_file &&other) noexcept
	{}

	inline constexpr ~basic_path_dir_file()
	{
	}
};

} // namespace fast_io
