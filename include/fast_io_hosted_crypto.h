#pragma once

#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4061 )
#pragma warning( disable : 4514 )
#pragma warning( disable : 4623 )
#pragma warning( disable : 4626 )
#pragma warning( disable : 4668 )
#pragma warning( disable : 4710 )
#pragma warning( disable : 4820 )
#pragma warning( disable : 5027 )
#pragma warning( disable : 5045 )
#endif

#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#include"fast_io_hosted.h"
#include"fast_io_crypto/platforms/impl.h"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif

#endif
