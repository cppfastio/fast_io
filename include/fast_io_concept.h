#pragma once
// fast_io_concept.h allows you define your device and type without compilation time penalty
#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif
#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++26 standard compiler."
#else

#include <version>
#include <cstddef>
#include <type_traits>
#include <concepts>
#include <cstdint>
#ifdef __HERBCEPTIONS__
#include <herbceptions/error>
#else
#if defined(_LIBCPP_VERSION) && \
	__has_include(<__system_error/errc.h>)
#include <__system_error/errc.h>
#elif defined(__GLIBCXX__) && \
	__has_include(<bits/error_constants.h>)
#include <bits/error_constants.h>
#elif defined(_MSVC_STL_UPDATE) && \
	__has_include(<xerrc.h>)
#include <xerrc.h>
#else
#include <system_error>
#endif
#endif

#include "fast_io_dsal/impl/misc/push_macros.h"
#include "fast_io_dsal/impl/misc/push_warnings.h"

#include "fast_io_core_impl/freestanding/herbceptions.h"
#include "fast_io_core_impl/freestanding/addressof.h"
#include "fast_io_core_impl/concepts/impl.h"

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
