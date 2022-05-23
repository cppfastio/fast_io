#pragma once

#include"width.h"

#include"boolalpha.h"
#include"integers.h"

#if __cpp_lib_chrono >= 201907L || (__GNUC__  >= 11 || defined(__clang__))
#if defined(_GLIBCXX_CHRONO) || defined(_LIBCPP_CHRONO) || defined(_CHRONO_)
#include"chrono.h"
#endif
#endif


#include"lc_time.h"
#include"cond.h"
