#pragma once

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#include"common.h"
#include"win32.h"
#else
#ifndef __wasi__
#include"common.h"
#endif

#if !defined(__MSDOS__)&& \
	!defined(_PICOLIBC)&& \
	(!defined(__NEWLIB__) || defined(__CYGWIN__))
#include"scatter.h"
#include"preadwrite.h"
#endif

#endif
#include"seek.h"
