#pragma once

/*

GCC provides more headers in freestanding environment than standard requires because WG21 sucks very badly.
no std::array, std::copy and others

*/

#include<iterator>
#include<ranges>

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
