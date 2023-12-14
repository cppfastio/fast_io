#pragma once

#include"native_base.h"
#if defined(_WIN32) || defined(__CYGWIN__)
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0501
#include"nt_mapping.h"
#endif
#include"win32_mapping.h"
#if defined(__CYGWIN__)
#include"posix_mapping.h"
#endif
#include"omap.h"
#elif !defined(__NEWLIB__) && !defined(__MSDOS__) && (!defined(__wasm__) || (defined(__wasi__)&&defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>)
#include"posix_mapping.h"
#include"omap.h"
#endif
