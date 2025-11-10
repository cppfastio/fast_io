#pragma once

#if (defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)
#include "win32.h"
#ifndef _WIN32_WINDOWS
#include "nt.h"
#endif
#elif defined (__linux__)

#endif
