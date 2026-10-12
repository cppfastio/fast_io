#pragma once

namespace fast_io
{

template <::std::integral char_type>
using basic_win32_io_observer_9xa = basic_win32_family_io_observer<win32_family::ansi_9x, char_type>;
template <::std::integral char_type>
using basic_win32_file_9xa = basic_win32_family_file<win32_family::ansi_9x, char_type>;
template <::std::integral char_type>
using basic_win32_pipe_9xa = basic_win32_family_pipe<win32_family::ansi_9x, char_type>;

using win32_io_observer_9xa = basic_win32_io_observer_9xa<char>;
using win32_file_9xa = basic_win32_file_9xa<char>;
using win32_pipe_9xa = basic_win32_pipe_9xa<char>;

using wwin32_io_observer_9xa = basic_win32_io_observer_9xa<wchar_t>;
using wwin32_file_9xa = basic_win32_file_9xa<wchar_t>;
using wwin32_pipe_9xa = basic_win32_pipe_9xa<wchar_t>;

using u8win32_io_observer_9xa = basic_win32_io_observer_9xa<char8_t>;
using u8win32_file_9xa = basic_win32_file_9xa<char8_t>;
using u8win32_pipe_9xa = basic_win32_pipe_9xa<char8_t>;

using u16win32_io_observer_9xa = basic_win32_io_observer_9xa<char16_t>;
using u16win32_file_9xa = basic_win32_file_9xa<char16_t>;
using u16win32_pipe_9xa = basic_win32_pipe_9xa<char16_t>;

using u32win32_io_observer_9xa = basic_win32_io_observer_9xa<char32_t>;
using u32win32_file_9xa = basic_win32_file_9xa<char32_t>;
using u32win32_pipe_9xa = basic_win32_pipe_9xa<char32_t>;

template <::std::integral char_type>
using basic_win32_io_observer_ntw = basic_win32_family_io_observer<win32_family::wide_nt, char_type>;
template <::std::integral char_type>
using basic_win32_file_ntw = basic_win32_family_file<win32_family::wide_nt, char_type>;
template <::std::integral char_type>
using basic_win32_pipe_ntw = basic_win32_family_pipe<win32_family::wide_nt, char_type>;

using win32_io_observer_ntw = basic_win32_io_observer_ntw<char>;
using win32_file_ntw = basic_win32_file_ntw<char>;
using win32_pipe_ntw = basic_win32_pipe_ntw<char>;

using wwin32_io_observer_ntw = basic_win32_io_observer_ntw<wchar_t>;
using wwin32_file_ntw = basic_win32_file_ntw<wchar_t>;
using wwin32_pipe_ntw = basic_win32_pipe_ntw<wchar_t>;

using u8win32_io_observer_ntw = basic_win32_io_observer_ntw<char8_t>;
using u8win32_file_ntw = basic_win32_file_ntw<char8_t>;
using u8win32_pipe_ntw = basic_win32_pipe_ntw<char8_t>;

using u16win32_io_observer_ntw = basic_win32_io_observer_ntw<char16_t>;
using u16win32_file_ntw = basic_win32_file_ntw<char16_t>;
using u16win32_pipe_ntw = basic_win32_pipe_ntw<char16_t>;

using u32win32_io_observer_ntw = basic_win32_io_observer_ntw<char32_t>;
using u32win32_file_ntw = basic_win32_file_ntw<char32_t>;
using u32win32_pipe_ntw = basic_win32_pipe_ntw<char32_t>;

template <::std::integral char_type>
using basic_win32_io_observer = basic_win32_family_io_observer<win32_family::native, char_type>;
template <::std::integral char_type>
using basic_win32_file = basic_win32_family_file<win32_family::native, char_type>;
template <::std::integral char_type>
using basic_win32_pipe = basic_win32_family_pipe<win32_family::native, char_type>;

using win32_io_observer = basic_win32_io_observer<char>;
using win32_file = basic_win32_file<char>;
using win32_pipe = basic_win32_pipe<char>;

using wwin32_io_observer = basic_win32_io_observer<wchar_t>;
using wwin32_file = basic_win32_file<wchar_t>;
using wwin32_pipe = basic_win32_pipe<wchar_t>;

using u8win32_io_observer = basic_win32_io_observer<char8_t>;
using u8win32_file = basic_win32_file<char8_t>;
using u8win32_pipe = basic_win32_pipe<char8_t>;

using u16win32_io_observer = basic_win32_io_observer<char16_t>;
using u16win32_file = basic_win32_file<char16_t>;
using u16win32_pipe = basic_win32_pipe<char16_t>;

using u32win32_io_observer = basic_win32_io_observer<char32_t>;
using u32win32_file = basic_win32_file<char32_t>;
using u32win32_pipe = basic_win32_pipe<char32_t>;

/* IOCP completion-port scheduler: open the file with fi::io_async */
using io_async_observer = win32_io_observer;
using io_async_scheduler = win32_file;

} // namespace fast_io
