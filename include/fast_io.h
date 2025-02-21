#pragma once
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else

#include "fast_io_hosted.h"

#include "fast_io_dsal/impl/misc/push_warnings.h"
#include "fast_io_dsal/impl/misc/push_macros.h"

#include "fast_io_legacy_impl/io.h"

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
