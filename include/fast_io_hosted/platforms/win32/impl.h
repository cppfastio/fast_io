#pragma once

#include"win32_definitions.h"
#include"apis.h"

#if defined(_MSC_VER) && !defined(_WIN32_WINDOWS) && !defined(_KERNEL_MODE)
#pragma comment(lib,"bcrypt.lib")
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#include"msvc_linker.h"
#endif

#include"utils.h"
