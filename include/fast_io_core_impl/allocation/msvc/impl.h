﻿#pragma once

#if SIZE_MAX > UINT_LEAST32_MAX
#include "msvc_linker_64.h"
#if defined(__arm64ec__) || defined(_M_ARM64EC)
#error "We do not support ARM64EC. See reasons: https://github.com/trcrsired/Why-Microsoft-ARM64EC-ABI-MUST-DIE/blob/main/README.md"
#include "msvc_linker_arm64ec.h"
#endif
#elif defined(__x86__) || defined(_M_IX86) || defined(__i386__)
#include "msvc_linker_32_i686.h"
#else
#include "msvc_linker_32.h"
#endif
