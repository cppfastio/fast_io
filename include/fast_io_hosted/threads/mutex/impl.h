#pragma once

#include"single.h"
#ifdef __USING_MCFGTHREAD__
#include <mcfgthread/mutex.h>
#include "mcf_gthread_mutex.h"
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
#include"rtl_critical_section.h"
#include"win32_critical_section.h"
#endif
#if !defined(__SINGLE_THREAD__) && (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_WIN32) && !defined(__MSDOS__) && !defined(__wasi__) && __has_include(<pthread.h>)
#include"posix_pthread_mutex.h"
#endif

namespace fast_io
{

using native_mutex =
#ifdef __USING_MCFGTHREAD__
mcf_gthread_mutex
#elif (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
win32_critical_section
#elif defined(__SINGLE_THREAD__) || defined(__MSDOS__) || (defined(__NEWLIB__) && !defined(__CYGWIN__)) || defined(__wasi__) || !__has_include(<pthread.h>)
single_thread_noop_mutex
#else
posix_pthread_mutex
#endif
;

}
