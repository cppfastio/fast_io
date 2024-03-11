#include <fast_io_dsal/deque.h>

auto test(::fast_io::deque<::std::size_t>::iterator a, ::fast_io::deque<::std::size_t>::iterator b)
{
	return a - b;
}