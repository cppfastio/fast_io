#pragma once

namespace fast_io
{

template <::std::integral chartype>
using basic_posix_io_observer = basic_posix_family_io_observer<::fast_io::posix_family::api, chartype>;
template <::std::integral chartype>
using basic_posix_file = basic_posix_family_file<::fast_io::posix_family::api, chartype>;
template <::std::integral chartype>
using basic_posix_pipe = basic_posix_family_pipe<::fast_io::posix_family::api, chartype>;

using posix_io_observer = basic_posix_io_observer<char>;
using posix_file = basic_posix_file<char>;
using posix_pipe = basic_posix_pipe<char>;

using wposix_io_observer = basic_posix_io_observer<wchar_t>;
using wposix_file = basic_posix_file<wchar_t>;
using wposix_pipe = basic_posix_pipe<wchar_t>;

using u8posix_io_observer = basic_posix_io_observer<char8_t>;
using u8posix_file = basic_posix_file<char8_t>;
using u8posix_pipe = basic_posix_pipe<char8_t>;

using u16posix_io_observer = basic_posix_io_observer<char16_t>;
using u16posix_file = basic_posix_file<char16_t>;
using u16posix_pipe = basic_posix_pipe<char16_t>;

using u32posix_io_observer = basic_posix_io_observer<char32_t>;
using u32posix_file = basic_posix_file<char32_t>;
using u32posix_pipe = basic_posix_pipe<char32_t>;

inline constexpr int posix_stdin_number{0};
inline constexpr int posix_stdout_number{1};
inline constexpr int posix_stderr_number{2};

template <::std::integral char_type = char>
inline constexpr basic_posix_io_observer<char_type> posix_stdin() noexcept
{
	return {posix_stdin_number};
}
template <::std::integral char_type = char>
inline constexpr basic_posix_io_observer<char_type> posix_stdout() noexcept
{
	return basic_posix_io_observer<char_type>{posix_stdout_number};
}
template <::std::integral char_type = char>
inline constexpr basic_posix_io_observer<char_type> posix_stderr() noexcept
{
	return basic_posix_io_observer<char_type>{posix_stderr_number};
}

#ifdef __MSDOS__

template <::std::integral chartype>
using basic_dos_io_observer = basic_posix_family_io_observer<::fast_io::posix_family::dos, chartype>;
template <::std::integral chartype>
using basic_dos_file = basic_posix_family_file<::fast_io::posix_family::dos, chartype>;
template <::std::integral chartype>
using basic_dos_pipe = basic_posix_family_pipe<::fast_io::posix_family::dos, chartype>;

using dos_io_observer = basic_dos_io_observer<char>;
using dos_file = basic_dos_file<char>;
using dos_pipe = basic_dos_pipe<char>;

using wdos_io_observer = basic_dos_io_observer<wchar_t>;
using wdos_file = basic_dos_file<wchar_t>;
using wdos_pipe = basic_dos_pipe<wchar_t>;

using u8dos_io_observer = basic_dos_io_observer<char8_t>;
using u8dos_file = basic_dos_file<char8_t>;
using u8dos_pipe = basic_dos_pipe<char8_t>;

using u16dos_io_observer = basic_dos_io_observer<char16_t>;
using u16dos_file = basic_dos_file<char16_t>;
using u16dos_pipe = basic_dos_pipe<char16_t>;

using u32dos_io_observer = basic_dos_io_observer<char32_t>;
using u32dos_file = basic_dos_file<char32_t>;
using u32dos_pipe = basic_dos_pipe<char32_t>;

template <::std::integral char_type = char>
inline constexpr basic_dos_io_observer<char_type> dos_stdin() noexcept
{
	return {posix_stdin_number};
}
template <::std::integral char_type = char>
inline constexpr basic_dos_io_observer<char_type> dos_stdout() noexcept
{
	return basic_dos_io_observer<char_type>{posix_stdout_number};
}
template <::std::integral char_type = char>
inline constexpr basic_dos_io_observer<char_type> dos_stderr() noexcept
{
	return basic_dos_io_observer<char_type>{posix_stderr_number};
}

#endif

#if !defined(_WIN32) || defined(__WINE__)
template <::std::integral char_type = char>
inline constexpr basic_posix_io_observer<char_type> native_stdin() noexcept
{
	return {posix_stdin_number};
}
template <::std::integral char_type = char>
inline constexpr basic_posix_io_observer<char_type> native_stdout() noexcept
{
	return basic_posix_io_observer<char_type>{posix_stdout_number};
}
template <::std::integral char_type = char>
inline constexpr basic_posix_io_observer<char_type> native_stderr() noexcept
{
	return basic_posix_io_observer<char_type>{posix_stderr_number};
}
#endif
} // namespace fast_io
