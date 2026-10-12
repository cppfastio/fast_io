#pragma once

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
#include "wincrt.h"
#elif defined(__AVR__) || defined(_PICOLIBC__)
#include "avrlibc.h"
#include "macros_general.h"
#else
#if defined(__LLVM_LIBC_TYPES_FILE_H__)
#include "llvm.h"
#elif defined(__UCLIBC__)
#if defined(__STDIO_BUFFERS)
#include "uclibc.h"
#elif defined(FAST_IO_LIBC_CUSTOM_BUFFER_PTRS)
#include "custom.h"
#endif
#elif defined(__MLIBC_O_CLOEXEC)
#include "mlibc.h"
#elif defined(__GLIBC__)
#include "glibc.h"
#elif defined(__wasi__)
#include "musl.h"
#elif defined(__NEED___isoc_va_list) || defined(__musl__)
#include "musl.h"
#elif defined(__serenity__)
#include "serenity.h"
#elif defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL) || (defined(__NEWLIB__) && !defined(__CUSTOM_FILE_IO__)) || \
	defined(__BIONIC__) || defined(__MSDOS__)
#include "unix.h"
#elif defined(FAST_IO_LIBC_CUSTOM_BUFFER_PTRS)
#include "custom.h"
#endif

#if !defined(__MSDOS__)
#include "general.h"
#endif
#include "done.h"
#include "macros_general.h"
#endif
