#pragma once
#if defined(__BIONIC__) && __has_include(<android/log.h>)
#include"android.h"
#elif ((defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__))
#include"nt_dbg.h"
#if defined(_WIN32_WINDOWS)
#include"err.h"
#endif
#else
#include"err.h"
#endif
