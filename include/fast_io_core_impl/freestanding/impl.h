#pragma once

/*

GCC provides more headers in freestanding environment than standard requires because WG21 sucks very badly.
no std::array, std::copy and others

*/
#if defined(__GLIBCXX__)
#if __has_include(<bits/ranges_base.h>) && __has_include(<bits/stl_iterator.h>)
#include<initializer_list>
#include<bits/stl_iterator.h>
#include<bits/ranges_base.h>
#else
#include<iterator>
#endif
#else
#include<iterator>
#endif

#if defined(__GLIBCXX__)
#include<bits/ptr_traits.h>
#else
#include<memory>
#endif

#if __cpp_lib_ranges >= 201911L
#include<ranges>
#endif

#include"array.h"
#include"cstr_len.h"
#include"bytes.h"
#include"algorithm.h"
#include"relocatable.h"
