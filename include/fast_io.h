#pragma once
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else
#include"fast_io_hosted.h"

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4514 )
#endif
#include"fast_io_legacy_impl/io.h"
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
