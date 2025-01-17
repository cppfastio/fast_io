#pragma once

#if !defined(__MSDOS__) && !defined(__NEWLIB__) && !defined(__wasi__) && !defined(_PICOLIBC__)
#include "option.h"
#include "arg_env.h"
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#include "win32.h"
#include "nt.h"
#endif

#if defined(__CYGWIN__) || ((!defined(_WIN32) || defined(__WINE__)) && __has_include(<sys/wait.h>))
#include <sys/wait.h>
#include "posix.h"
#endif

namespace fast_io
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#if !defined(_WIN32_WINDOWS) && (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x600)
using native_wait_status = ::fast_io::nt_wait_status;
using native_process_args = ::fast_io::nt_process_args;
using native_process_envs = ::fast_io::nt_process_envs;
using native_process_observer = ::fast_io::nt_process_observer;
using native_process = ::fast_io::nt_process;
#else
using native_wait_status = ::fast_io::win32_wait_status;
using native_process_args = ::fast_io::win32_process_args;
using native_process_envs = ::fast_io::win32_process_envs;
using native_process_observer = ::fast_io::win32_process_observer;
using native_process = ::fast_io::win32_process;
#endif
#else
using native_wait_status = ::fast_io::posix_wait_status;
using native_process_args = ::fast_io::posix_process_args;
using native_process_envs = ::fast_io::posix_process_envs;
using native_process_observer = ::fast_io::posix_process_observer;
using native_process = ::fast_io::posix_process;
#endif
} // namespace fast_io

#endif
