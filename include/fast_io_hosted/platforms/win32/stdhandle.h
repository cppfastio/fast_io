#pragma once

namespace fast_io
{

template <::std::integral char_type = char>
inline basic_win32_io_observer_9xa<char_type> win32_stdin_9xa() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer_9xa<char_type> win32_stdout_9xa() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer_9xa<char_type> win32_stderr_9xa() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stderr_number)};
}

template <::std::integral char_type = char>
inline basic_win32_io_observer_ntw<char_type> win32_stdin_ntw() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer_ntw<char_type> win32_stdout_ntw() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer_ntw<char_type> win32_stderr_ntw() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stderr_number)};
}

template <::std::integral char_type = char>
inline basic_win32_io_observer<char_type> win32_stdin() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer<char_type> win32_stdout() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer<char_type> win32_stderr() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stderr_number)};
}

#if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)
template <::std::integral char_type = char>
inline basic_win32_io_observer<char_type> native_stdin() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdin_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer<char_type> native_stdout() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stdout_number)};
}
template <::std::integral char_type = char>
inline basic_win32_io_observer<char_type> native_stderr() noexcept
{
	return {::fast_io::win32::GetStdHandle(win32_stderr_number)};
}
#endif

namespace freestanding
{
template <>
struct is_trivially_copyable_or_relocatable<win32_file_factory>
{
	inline static constexpr bool value = true;
};

template <>
struct is_trivially_copyable_or_relocatable<win32_9xa_dir_file_factory>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_win32_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
