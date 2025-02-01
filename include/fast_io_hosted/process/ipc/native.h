#pragma once

#if !defined(__MSDOS__) && !defined(__NEWLIB__) && !defined(__wasi__) && !defined(_PICOLIBC__)
#include "basic.h"

#include "posix/named_pipe.h"
#include "posix/eventfd.h"
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#include "win32/named_pipe_win32.h"
#include "win32/rpc_nt.h"
#include "win32/alpc_nt.h"
#endif

namespace fast_io
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#if !defined(_WIN32_WINDOWS) && (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x600)
	// alpc
#elif !defined(_WIN32_WINDOWS)
	// rpc
#else
	// named pipe
template <::std::integral char_type>
using basic_native_ipc_observer = ::fast_io::basic_named_pipe_ipc_observer<char_type>;
#endif
#else
	// posix to do
#endif
}
#endif
