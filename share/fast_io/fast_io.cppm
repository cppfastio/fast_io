module;

#include <fast_io_dsal/array.h>
#include <fast_io_dsal/vector.h>

#if !(((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES)))
#ifndef FAST_IO_FREESTANDING
#define FAST_IO_FREESTANDING
#endif
#endif

export module fast_io;

#include "inc/fast_io_dsal/array.inc"
#include "inc/fast_io_dsal/vector.inc"
