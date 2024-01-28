#pragma once

#if !defined(__MSDOS__) && !defined(__NEWLIB__) && !defined(__wasi__) && !defined(_PICOLIBC__)
#include "base.h"
#if (defined(_WIN32) && !defined(_WIN32_WINDOWS) && !defined(__WINE__)) || defined(__CYGWIN__)
#include "nt.h"
#endif

#if defined(__CYGWIN__) || ((!defined(_WIN32) || defined(__WINE__)) && __has_include(<sys/wait.h>))
#include <sys/wait.h>
#include "posix.h"
#endif

namespace fast_io
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#if !defined(_WIN32_WINDOWS)
using native_process_observer = ::fast_io::nt_process_observer;
using native_process = ::fast_io::nt_process;
#endif
#else
using native_process_observer = ::fast_io::posix_process_observer;
using native_process = ::fast_io::posix_process;
#endif
} // namespace fast_io

#endif
