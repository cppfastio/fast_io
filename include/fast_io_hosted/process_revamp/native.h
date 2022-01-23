#pragma once

#if !defined(__MSDOS__) && !defined(__NEWLIB__) && !defined(__wasi__) && !defined(_PICOLIBC__)

#if defined(_WIN32)
#include"nt.h"
#elif __has_include(<sys/wait.h>)
#include <sys/wait.h>
#include"posix.h"
#endif
#endif

