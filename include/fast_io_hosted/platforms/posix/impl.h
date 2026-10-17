#pragma once

#if ((defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)) || defined(__MSDOS__)
#if __has_include(<corecrt_io.h>)
#include <corecrt_io.h>
#elif __has_include(<io.h>)
#include <io.h>
#endif
#endif

#if __has_include(<fcntl.h>)
#include <fcntl.h>
#endif

#if __has_include(<sys/stat.h>)
#include <sys/stat.h>
#endif

#if (!defined(_WIN32) || defined(__WINE__) || defined(__BIONIC__)) || defined(__CYGWIN__)

#if __has_include(<features.h>)
#include <features.h>
#endif

#if __has_include(<unistd.h>)
#include <unistd.h>
#endif

#if __has_include(<sys/uio.h>)
#include <sys/uio.h>
#endif

#if __has_include(<sys/types.h>)
#include <sys/types.h>
#endif

#if __has_include(<sys/socket.h>)
#include <sys/socket.h>
#endif

#if __has_include(<signal.h>) && 0
#include <signal.h>
#endif

#if defined(__wasi__)
#include <wasi/api.h>
#endif

#endif
#include "../systemcall_details.h"

#if ((!defined(_WIN32) || defined(__WINE__)) || defined(__CYGWIN__)) && \
	__has_include(<sys/socket.h>) && __has_include(<netinet/in.h>)
#include <netinet/in.h>
#include "../posix_netmode.h"
#endif

#if defined(F_RDLCK) && defined(F_WRLCK) && defined(F_UNLCK) && !defined(__wasi__)
#include "../posix_file_lock.h"
#endif

#if defined(__MSDOS__)
#include <libc/fd_props.h>
#endif

#include "open.h"
#include "io_observer.h"
#include "file.h"
#include "pipe.h"
#include "aliases.h"

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
#include "common.h"
#include "win32.h"
#else
#ifndef __wasi__
#include "common.h"
#endif

#if !defined(__MSDOS__) && !defined(_PICOLIBC) && (!defined(__NEWLIB__) || defined(__CYGWIN__))
#include "scatter.h"
#if !(defined(_WIN32) && defined(__BIONIC__)) && !defined(__CYGWIN__) && !defined(__NEWLIB__)
#include "preadwrite.h"
#endif
#endif

#endif
#include "seek.h"
#include "transmit.h"

#if defined(__linux__) && defined(__HERBCEPTIONS__)
#include "../posix_mapping.h"
#include "async/linux_liburing.h"
#include "async/linux_io_uring.h"
#endif

/* thread-pool fallback scheduler — portable across every POSIX target;
 * on Linux io_uring remains the io_async default while the pool stays
 * available explicitly, elsewhere io_async maps to the pool. wasi gets
 * the pool only when the target has the wasm threads feature; a plain
 * single-threaded wasi build has no pthreads to run the workers. */
#if !defined(_WIN32) && !defined(__MSDOS__) && (!defined(__wasi__) || defined(FAST_IO_HAS_WASM_THREADS)) && defined(__HERBCEPTIONS__)
#include "async/thread_pool.h"
#endif
