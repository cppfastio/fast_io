#pragma once

#if defined(_LIBCPP_VERSION)
#include"libc++.h"
#elif defined(__GLIBCXX__)
#include"libstdc++.h"
#elif defined(_MSVC_STL_UPDATE)
#include"msvc_stl.h"
#else
#error "Anyone knows how to hack brutal std::basic_string internal on this platform?"
#endif

#include"ostring_ref.h"
#include"concat.h"
#include"scan_str_ctx_impl.h"
