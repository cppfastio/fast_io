#pragma once

#if (defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)
#include "win32.h"
#ifndef _WIN32_WINDOWS
#include "nt.h"
#endif
#elif !defined(__SINGLE_THREAD__) && !defined(__NEWLIB__) && \
	!defined(__MSDOS__) && !defined(__wasi__) && __has_include(<pthread.h>)
#include "pthread.h"
#endif
