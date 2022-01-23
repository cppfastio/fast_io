#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#include "win32.h"
#endif
#if (!defined(_WIN32)|| defined(__WINE__)) && (__has_include(<dlfcn.h>)&&(defined(__CYGWIN__) || (!defined(__NEWLIB__)&&!defined(__wasi__))))
#include <dlfcn.h>
#include "posix.h"
#endif
