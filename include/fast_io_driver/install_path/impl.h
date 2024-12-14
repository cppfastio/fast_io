#pragma once

#include"install_path.h"

#if defined(_WIN32)
#include "argv0_null.h"
#else
#include "argv0.h"
#endif

#if (defined(__linux) || defined(__linux__) || defined(__gnu_linux__)) || defined(__CYGWIN__) || defined(__sun)
#include "linux.h"
#elif defined(_WIN32)
#if defined(_WIN32_WINDOWS)
#include "win32_9xa.h"
#else
#include "nt.h"
#endif
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(BSD) || defined(_SYSTYPE_BSD)
#include "bsd.h"
#elif defined(__OpenBSD__)
#include "openbsd.h"
#elif defined(__APPLE__) || defined(__DARWIN_C_LEVEL)
#include "darwin.h"
#else
#include "null.h"
#endif

#include "method.h"
