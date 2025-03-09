#pragma once

#if !defined(__MSDOS__) && !defined(__NEWLIB__) && !defined(__wasi__) && !defined(_PICOLIBC__)
#include "mode.h"
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#include "win32/named_pipe_win32.h"
#include "win32/alpc_nt.h"
#else
// #include "posix/named_pipe.h"
// #include "posix/eventfd.h"
#endif

namespace fast_io
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#if !defined(_WIN32_WINDOWS) && (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x600)
	// alpc
#else
	// named pipe
#endif
#else
	// posix to do
#endif
}
#endif
