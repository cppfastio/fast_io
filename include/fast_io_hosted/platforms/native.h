#pragma once
#include"native_base.h"
#if defined(_WIN32) || defined(__CYGWIN__)
#include"win32_mapping.h"
#if defined(__CYGWIN__)
#include"posix_mapping.h"
#endif
#include"omap.h"
#elif !defined(__NEWLIB__) && !defined(__MSDOS__) && (!defined(__wasm__) || (defined(__wasi__)&&defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>)
#include"posix_mapping.h"
#include"omap.h"
#endif
