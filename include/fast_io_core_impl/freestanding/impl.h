#pragma once

/*

GCC provides more headers in freestanding environment than standard requires because WG21 sucks very badly.
no std::array, std::copy and others

*/

#include"array.h"
#include"cstr_len.h"
#include"bytes.h"
#include"algorithm.h"
#include"relocatable.h"
