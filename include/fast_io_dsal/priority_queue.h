#pragma once
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
#include "vector.h"
#include "impl/priority_queue.h"
#include "impl/misc/pop_warnings.h"
#include "impl/misc/pop_macros.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <typename T, typename Cmp = ::std::ranges::less, typename Container = ::fast_io::vector<T>>
using priority_queue = ::fast_io::containers::priority_queue<Cmp, Container>;

using zu_priority_queue = ::fast_io::containers::priority_queue<::std::ranges::less, ::fast_io::vector<::std::size_t>>;

namespace tlc
{
template <typename T, typename Cmp = ::std::ranges::less, typename Container = ::fast_io::tlc::vector<T>>
using priority_queue = ::fast_io::containers::priority_queue<Cmp, Container>;

using zu_priority_queue = ::fast_io::containers::priority_queue<::std::ranges::less, ::fast_io::tlc::vector<::std::size_t>>;
} // namespace tlc

} // namespace fast_io
#endif
