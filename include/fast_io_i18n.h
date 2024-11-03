#pragma once
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else
#include "fast_io_hosted.h"

#include "fast_io_dsal/impl/misc/push_warnings.h"

#include "fast_io_i18n/lc.h"
#include "fast_io_i18n/lc_print.h"
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#include "fast_io_i18n/locale/impl.h"
#endif
#include "fast_io_i18n/imbuer.h"
#include "fast_io_i18n/lc_status_print.h"
#include "fast_io_i18n/lc_numbers/impl.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#if defined(_GLIBCXX_STRING) || defined(_LIBCPP_STRING) || defined(_STRING_)
#include "fast_io_unit/string.h"
#include "fast_io_unit/string_impl/lc_concat.h"
#endif
#endif

#ifndef FAST_IO_DISABLE_FLOATING_POINT
#include "fast_io_unit/floating/lc_impl.h"
#endif

#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
