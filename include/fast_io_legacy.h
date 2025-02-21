#pragma once
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else
// fast_io_legacy.h deals with legacy C++ <iostream>/<fstream>/<sstream> interface
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))


#include <streambuf>
#include <fstream>
#include <sstream>

#include "fast_io.h"
#include "fast_io_dsal/impl/misc/push_warnings.h"
#include "fast_io_dsal/impl/misc/push_macros.h"
#include "fast_io_legacy_impl/filebuf/streambuf_io_observer.h"
#if !defined(_LIBCPP_HAS_NO_FILESYSTEM) || defined(_LIBCPP_HAS_FSTREAM)
#include "fast_io_legacy_impl/filebuf/filebuf_file.h"
#endif
#include "fast_io_legacy_impl/filebuf/op_out.h"

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif

#endif
