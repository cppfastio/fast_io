#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#if !defined(_M_ARM64EC)
#include "x86.h"
#endif
#include "arm.h"
#include "common.h"
#endif
