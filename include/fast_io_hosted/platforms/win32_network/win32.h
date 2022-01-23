#pragma once

#if defined(_MSC_VER) && !defined(_KERNEL_MODE)
#pragma comment(lib,"ws2_32.lib")
#endif

#include"service.h"
#include"socket_file.h"
#include"win32_dns.h"

#if !defined(_MSC_VER) && !defined(__WINE__) && !defined(__CYGWIN__)
#include"win32_9x_dns.h"
#endif
