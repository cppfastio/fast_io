#pragma once

#include"file_size.h"
#include"allocation_file_loader.h"
#if defined(_WIN32) || defined(__CYGWIN__)
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0501
#include"nt_file_loader.h"
#endif
#include"win32_file_loader.h"
#endif

#if !defined(_WIN32) && !defined(__MSDOS__) && (!defined(__wasm__) || defined(_WASI_EMULATED_MMAN)) && !defined(_PICOLIBC__)
#include"posix_file_loader.h"
#endif

namespace fast_io
{

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
using native_file_loader = win32_file_loader;
using native_mmap_options = win32_mmap_options;
#elif (!defined(__NEWLIB__)||defined(__CYGWIN__)) && !defined(__MSDOS__) && !defined(__wasm__) && !defined(_PICOLIBC__)
using native_file_loader = posix_file_loader;
using native_mmap_options = posix_mmap_options;
#else
using native_file_loader = allocation_file_loader;
using native_mmap_options = allocation_mmap_options;
#endif

}
