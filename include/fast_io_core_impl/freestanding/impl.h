#pragma once

/*

GCC provides more headers in freestanding environment than standard requires because WG21 sucks very badly.
no std::array, std::copy and others

*/
#if defined(__GLIBCXX__) &&__has_include(<bits/ranges_base.h>) && __has_include(<bits/stl_iterator.h>) && _GLIBCXX_RELEASE >= 12
#include<initializer_list>
#include<bits/stl_iterator.h>
#include<bits/ranges_base.h>
#else
#include<iterator>
#include<ranges>
#endif

#if defined(__GLIBCXX__) &&__has_include(<bits/move.h>)
#include<bits/move.h>
#else
#include<utility>
#endif

#if defined(__GLIBCXX__) && __has_include(<bits/ptr_traits.h>)
#include<bits/ptr_traits.h>
#else
#include<memory>
#endif

#include"array.h"
#include"cstr_len.h"
#include"bytes.h"
#include"algorithm.h"
#include"relocatable.h"
