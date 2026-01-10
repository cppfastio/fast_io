#pragma once

// std
#if __has_include(<chrono>)
#include <chrono>
#endif

#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>

#if (defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)
#include "win32.h"
#ifndef _WIN32_WINDOWS
#include "nt.h"
#endif
#elif defined(__MSDOS__) || defined(__DJGPP__)
#include <dpmi.h>
#include "dos.h"
#elif defined(__wasi__)
#include <wasi/api.h>
#include "wasi.h"
#elif !defined(__SINGLE_THREAD__) && !defined(__NEWLIB__) && !defined(__MSDOS__) && __has_include(<pthread.h>)
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
#include "pthread.h"
#elif defined(__NEWLIB__)
#include "newlib.h"
#endif
