module;

#include <fast_io.h>
#include <fast_io_legacy.h>
#include <fast_io_device.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>

#if !(((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
		!defined(_LIBCPP_FREESTANDING)) ||                                             \
	   defined(FAST_IO_ENABLE_HOSTED_FEATURES)))
#ifndef FAST_IO_FREESTANDING
#define FAST_IO_FREESTANDING
#endif
#endif

export module fast_io;

#include "fast_io_inc/core.inc"

#ifndef FAST_IO_FREESTANDING
#include "fast_io_inc/hosted.inc"
#include "fast_io_inc/legacy/c.inc"
#include "fast_io_inc/legacy/filebuf.inc"
#include "fast_io_inc/device.inc"
#endif

/*
io functions
*/
#include "fast_io_inc/io.inc"

/*
containers
*/
#include "fast_io_inc/dsal/array.inc"
#include "fast_io_inc/dsal/vector.inc"
#include "fast_io_inc/dsal/string.inc"
