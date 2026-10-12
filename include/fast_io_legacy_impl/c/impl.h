#pragma once

#if __has_include(<cstdio>)
#include <cstdio>
#else
#include <stdio.h>
#endif

#if defined(__MINGW32__) && !defined(_UCRT) && !defined(__BIONIC__)
#include "libc/msvcrt_lock.h"
#endif

#include "common.h"
#include "io_observer.h"
#include "file.h"
#include "aliases.h"
#include "libc/impl.h"
#include "preadwrite.h"
