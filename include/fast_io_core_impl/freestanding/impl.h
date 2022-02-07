#pragma once

/*

GCC provides more headers in freestanding environment than standard requires because WG21 sucks very badly.
no std::move, std::forward, std::addressof. WTF???

*/

#include"to_address.h"
#include"array.h"
#include"iterator.h"
#include"cstr_len.h"
#include"bytes.h"
#include"string_view.h"
#include"algorithm.h"

#if _LIBCPP_VERSION
#if __has_include(<__ranges/concepts.h>)
#include<__ranges/concepts.h>
#elif __has_include(<ranges>)
#include<ranges>
#endif
#endif
