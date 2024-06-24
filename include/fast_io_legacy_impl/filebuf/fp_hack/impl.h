#pragma once
#include "common.h"
#if defined(_LIBCPP_VERSION)
#include "libc++symbol.h"
#include "libc++.h"
#elif defined(__GLIBCXX__)
#include "libstdc++symbol.h"
#include "libstdc++.h"
#elif defined(_MSVC_STL_UPDATE)
#include "libstdc++symbol.h"
#include "msvc_stl.h"
#else
#include "generic.h"
#endif
