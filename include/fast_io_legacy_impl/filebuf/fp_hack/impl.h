#pragma once
#if defined(_LIBCPP_VERSION)
#include"libc++.h"
#elif defined(__GLIBCXX__)
#include"libstdc++.h"
#elif defined(_MSVC_STL_UPDATE)
#include"msvc_stl.h"
#endif