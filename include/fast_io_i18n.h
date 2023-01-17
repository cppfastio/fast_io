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
#pragma warning( disable : 4623 )
#pragma warning( disable : 4668 )
#pragma warning( disable : 4710 )
#pragma warning( disable : 4711 )
#pragma warning( disable : 4820 )
#pragma warning( disable : 5045 )
#endif
#include"fast_io_i18n/lc.h"
#include"fast_io_i18n/lc_print.h"
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#include"fast_io_i18n/locale/impl.h"
#endif
#include"fast_io_i18n/imbuer.h"
#include"fast_io_i18n/lc_print_status.h"
#include"fast_io_i18n/lc_numbers/impl.h"

#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#if defined(_GLIBCXX_STRING) || defined(_LIBCPP_STRING) || defined(_STRING_)
#include"fast_io_unit/string.h"
#include"fast_io_unit/string_impl/lc_concat.h"
#endif
#endif
#endif

#ifndef FAST_IO_DISABLE_FLOATING_POINT
#include"fast_io_unit/floating/lc_impl.h"
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
