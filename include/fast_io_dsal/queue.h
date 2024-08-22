#pragma once
#include "deque.h"
#include "impl/queue.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <typename T, typename Container = ::fast_io::deque<T>>
using queue = ::fast_io::containers::queue<Container>;

namespace tlc
{
template <typename T, typename Container = ::fast_io::tlc::deque<T>>
using queue = ::fast_io::containers::queue<Container>;
}

} // namespace fast_io
#endif