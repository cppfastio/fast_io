#pragma once

#if defined(_WIN32) || defined(__CYGWIN__) || defined(__WINE__)
#include "nt_options.h"
#include "nt.h"
#include "win32_options.h"
#include "win32.h"
#endif

#if (!defined(_WIN32) || defined(__CYGWIN__) || defined(__WINE__) || defined(__BIONIC__)) && !defined(__MSDOS__) &&    \
    !defined(_PICOLIBC) && (!defined(__wasi__) || defined(_WASI_EMULATED_MMAN))
#if __has_include(<sys/mman.h>)
#include <sys/mman.h>
#endif
#include "posix_options.h"
#include "posix.h"
#endif

#include "allocation_options.h"
