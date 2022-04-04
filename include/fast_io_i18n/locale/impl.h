#pragma once

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#include"win32.h"
#endif
#if !defined(__wasi__) || !defined(__NEWLIB__) || !defined(_WIN32) || defined(__WINE__) || defined(__CYGWIN__)
#include"posix.h"
#endif
