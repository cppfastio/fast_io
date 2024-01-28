#pragma once

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
