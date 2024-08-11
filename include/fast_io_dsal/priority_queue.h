#pragma once
#include "vector.h"
#include "impl/priority_queue.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <typename T, typename Cmp = ::std::ranges::less, typename Container = ::fast_io::vector<T>>
using priority_queue = ::fast_io::containers::priority_queue<Cmp, Container>;

namespace tlc
{
template <typename T, typename Cmp = ::std::ranges::less, typename Container = ::fast_io::tlc::vector<T>>
using priority_queue = ::fast_io::containers::priority_queue<Cmp, Container>;
}

} // namespace fast_io
#endif
