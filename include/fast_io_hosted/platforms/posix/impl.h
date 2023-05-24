#pragma once

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#include"common.h"
#include"win32.h"
#elif !defined(__MSDOS__)&& \
	!defined(_PICOLIBC)&& \
	(!defined(__NEWLIB__) || defined(__CYGWIN__))
#ifndef __wasi__
#include"common.h"
#endif
#include"scatter.h"
#include"preadwrite.h"
#endif
#include"seek.h"
