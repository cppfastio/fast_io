#pragma once

#if !defined(__MSDOS__) && !defined(__NEWLIB__) && !defined(__wasi__) && !defined(_PICOLIBC__)

#if defined(_WIN32) && !defined(_WIN32_WINDOWS)
#include"nt.h"
namespace fast_io {
using native_process_observer = ::fast_io::nt_process_observer;
using native_process = ::fast_io::nt_process;
}  // namespace fast_io

#elif __has_include(<sys/wait.h>)
#include <sys/wait.h>
#include"posix.h"
namespace fast_io {
using native_process_observer = ::fast_io::posix_process_observer;
using native_process = ::fast_io::posix_process;
}  // namespace fast_io
#endif
#endif
