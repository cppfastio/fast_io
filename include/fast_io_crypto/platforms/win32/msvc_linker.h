#pragma once

#if SIZE_MAX > UINT_LEAST32_MAX
#include"msvc_linker_64.h"
#elif defined(__x86__) || defined(_M_IX86) || defined(__i386__)
#include"msvc_linker_32_i686.h"
#else
#include"msvc_linker_32.h"
#endif
