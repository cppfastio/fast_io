#pragma once

#include"time.h"

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#include"win32_timezone.h"
#endif
#if !defined(_WIN32) || defined(__WINE__)
#if defined(__MSDOS__) || (defined(__NEWLIB__) && !defined(__CYGWIN__)) || defined(__wasi__)
#include"environ_timezone.h"
#else
#include"posix_timezone.h"
#endif
#endif
