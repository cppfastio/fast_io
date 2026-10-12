#pragma once

namespace fast_io
{

template <::std::integral char_type>
using basic_wine_io_observer = basic_wine_family_io_observer<wine_family::native, char_type>;

template <::std::integral char_type>
using basic_wine_file = basic_wine_family_file<wine_family::native, char_type>;

using wine_io_observer = basic_wine_io_observer<char>;
using wine_file = basic_wine_file<char>;

using wwine_io_observer = basic_wine_io_observer<wchar_t>;
using wwine_file = basic_wine_file<wchar_t>;

using u8wine_io_observer = basic_wine_io_observer<char8_t>;
using u8wine_file = basic_wine_file<char8_t>;

using u16wine_io_observer = basic_wine_io_observer<char16_t>;
using u16wine_file = basic_wine_file<char16_t>;

using u32wine_io_observer = basic_wine_io_observer<char32_t>;
using u32wine_file = basic_wine_file<char32_t>;

/*
std streams through whichever wineunix.dll is loaded: unixcall impl returns
unix fd + 1, nt impl returns the process's Standard{Input,Output,Error}
handle — both wrapped as a host_fd. infallible, so the status api is used
directly and the status dropped.
*/
template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stdin() noexcept
{
	return {::fast_io::wine::wine_unix_get_std_host_fd_returns_status(0).host_fd};
}

template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stdout() noexcept
{
	return {::fast_io::wine::wine_unix_get_std_host_fd_returns_status(1).host_fd};
}

template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stderr() noexcept
{
	return {::fast_io::wine::wine_unix_get_std_host_fd_returns_status(2).host_fd};
}

namespace freestanding
{
template <wine_family fm>
struct is_trivially_copyable_or_relocatable<wine_family_file_factory<fm>>
{
	inline static constexpr bool value = true;
};

template <wine_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_wine_family_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <wine_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_wine_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
