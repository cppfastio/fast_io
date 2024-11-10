#pragma once

#include "apis.h"
#include "fsutils.h"

#if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_WIN32) && !defined(__MSDOS__) && __has_include(<dirent.h>) && !defined(_PICOLIBC__)
#include "posix.h"
#include "posix_at.h"
#endif

#if (defined(_WIN32) || defined(__CYGWIN__))
#if defined(_WIN32_WINDOWS)
#include "../../fast_io_dsal/string.h"
#include "win9x.h"
#include "win9x_at.h"
#else
#include "nt.h"
#include "nt_at.h"
#endif
#endif

#if defined(__MSDOS__)
#include "dos.h"
#include "dos_at.h"
#endif
