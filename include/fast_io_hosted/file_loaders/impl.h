#pragma once

#include "file_size.h"
#include "allocation_file_loader.h"
#if defined(_WIN32) || defined(__CYGWIN__)
#include "nt_file_loader.h"
#include "win32_file_loader.h"
#endif

#if !defined(_WIN32) && !defined(__MSDOS__) && (!defined(__wasm__) || defined(_WASI_EMULATED_MMAN)) && \
	!defined(_PICOLIBC__) && (!defined(__NEWLIB__) || defined(__CYGWIN__))
#include "posix_file_loader.h"
#endif

namespace fast_io
{

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__) && !defined(__BIONIC__)
#if defined(_WIN32_WINDOWS)
using native_file_loader = win32_file_loader;
using native_mmap_options = win32_mmap_options;
#else
using native_file_loader = nt_file_loader;
using native_mmap_options = nt_mmap_options;
#endif
#elif (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(__MSDOS__) && !defined(__wasm__) && \
	!defined(_PICOLIBC__)
using native_file_loader = posix_file_loader;
using native_mmap_options = posix_mmap_options;
#else
using native_file_loader = allocation_file_loader;
using native_mmap_options = allocation_mmap_options;
#endif

} // namespace fast_io
