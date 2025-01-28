﻿#pragma once

#include "util.h"
#if defined(_WIN32) || defined(__CYGWIN__)
#include "win32/impl.h"
namespace fast_io
{

inline constexpr ::std::uint_least32_t win32_stdin_number(static_cast<::std::uint_least32_t>(-10));
inline constexpr ::std::uint_least32_t win32_stdout_number(static_cast<::std::uint_least32_t>(-11));
inline constexpr ::std::uint_least32_t win32_stderr_number(static_cast<::std::uint_least32_t>(-12));

} // namespace fast_io
#include "win32_code.h"
#include "win32_io_redirection.h"
#include "nt/impl.h"
#include "win32_error.h"

#include "nt.h"
// #include"win32_iocp_overlapped.h"

#include "win32.h"
#include "win32_network/win32.h"
#endif
#include "linux/system_call.h"
#include "posix.h"
#if (!defined(_WIN32) || defined(__WINE__)) && \
	__has_include(<sys/socket.h>) && __has_include(<netinet/in.h>) && !defined(__wasi__)
#include "posix_netop.h"
#if __has_include(<netdb.h>)
#include "posix_dns.h"
#endif
#endif

namespace fast_io
{

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
#if defined(_WIN32_WINDOWS)
using native_at_entry = win32_9xa_at_entry;
using native_fs_dirent = win32_9xa_fs_dirent;
#else
using native_at_entry = nt_at_entry;
using native_fs_dirent = nt_fs_dirent;
#endif

#if defined(_WIN32_WINDOWS)
template <::std::integral ch_type>
using basic_native_io_observer = basic_win32_io_observer<ch_type>;
template <::std::integral ch_type>
using basic_native_file = basic_win32_file<ch_type>;
template <::std::integral ch_type>
using basic_native_pipe = basic_win32_pipe<ch_type>;
#else
template <::std::integral ch_type>
using basic_native_io_observer = basic_nt_io_observer<ch_type>;
template <::std::integral ch_type>
using basic_native_file = basic_nt_file<ch_type>;
template <::std::integral ch_type>
using basic_native_pipe = basic_nt_pipe<ch_type>;
#endif
using native_process_io = win32_process_io;

#else

using native_at_entry = posix_at_entry;
using native_fs_dirent = posix_fs_dirent;

template <::std::integral ch_type>
using basic_native_io_observer = basic_posix_io_observer<ch_type>;
template <::std::integral ch_type>
using basic_native_file = basic_posix_file<ch_type>;
template <::std::integral ch_type>
using basic_native_pipe = basic_posix_pipe<ch_type>;
using native_process_io = posix_process_io;
#endif

using native_io_observer = basic_native_io_observer<char>;
using native_file = basic_native_file<char>;
using native_pipe = basic_native_pipe<char>;

using wnative_io_observer = basic_native_io_observer<wchar_t>;
using wnative_file = basic_native_file<wchar_t>;
using wnative_pipe = basic_native_pipe<wchar_t>;

using u8native_io_observer = basic_native_io_observer<char8_t>;
using u8native_file = basic_native_file<char8_t>;
using u8native_pipe = basic_native_pipe<char8_t>;

using u16native_io_observer = basic_native_io_observer<char16_t>;
using u16native_file = basic_native_file<char16_t>;
using u16native_pipe = basic_native_pipe<char16_t>;

using u32native_io_observer = basic_native_io_observer<char32_t>;
using u32native_file = basic_native_file<char32_t>;
using u32native_pipe = basic_native_pipe<char32_t>;

#if defined(_WIN32) || (__has_include(<sys/socket.h>) && __has_include(<netinet/in.h>) && !defined(__wasi__))
using native_socket_io_observer = basic_native_socket_io_observer<char>;
using native_socket_file = basic_native_socket_file<char>;

using wnative_socket_io_observer = basic_native_socket_io_observer<wchar_t>;
using wnative_socket_file = basic_native_socket_file<wchar_t>;

using u8native_socket_io_observer = basic_native_socket_io_observer<char8_t>;
using u8native_socket_file = basic_native_socket_file<char8_t>;

using u16native_socket_io_observer = basic_native_socket_io_observer<char16_t>;
using u16native_socket_file = basic_native_socket_file<char16_t>;

using u32native_socket_io_observer = basic_native_socket_io_observer<char32_t>;
using u32native_socket_file = basic_native_socket_file<char32_t>;
#endif

} // namespace fast_io
