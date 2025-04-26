module;

#include <fast_io.h>
#include <fast_io_legacy.h>

#if !(((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && !defined(_LIBCPP_FREESTANDING)) ||                                      \
       defined(FAST_IO_ENABLE_HOSTED_FEATURES)))
#ifndef FAST_IO_FREESTANDING
# define FAST_IO_FREESTANDING
#endif
#endif

export module fast_io_legacy;
export import fast_io;

#if !defined(FAST_IO_FREESTANDING) && (!defined(_LIBCPP_HAS_NO_FILESYSTEM) || defined(_LIBCPP_HAS_FSTREAM))
#include "fast_io_inc/legacy/filebuf.inc"
#endif
