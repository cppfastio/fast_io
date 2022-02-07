#pragma once

#include"apis.h"
#include"fsutils.h"

#if (!defined(__NEWLIB__)||defined(__CYGWIN__)) && !defined(_WIN32) && !defined(__MSDOS__) && __has_include(<dirent.h>) && !defined(_PICOLIBC__)
#include"posix.h"
#include"posix_at.h"
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#include"nt.h"
#include"nt_at.h"
#endif
