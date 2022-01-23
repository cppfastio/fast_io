#pragma once

namespace fast_io
{

enum class async
{
	unknown = 0,
	iocp = 1,
	epoll = 2,
	kqueue = 3,
	posix = 4,
	native =
#if defined(__WINNT__) || defined(_MSC_VER)
	iocp
#elif defined(__linux__)
	epoll
#elif defined(__FreeBSD__)
	kqueue
#else
	posix
#endif
};

}

#if defined(__WINNT__) || defined(_MSC_VER)
#include"iocp.h"
#elif defined(__linux__)
#include"epoll.h"
#endif