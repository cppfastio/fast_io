#include <cassert>
#include <algorithm>
#include <list>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	auto head = fast_io::vector<int>{1, 2, 3, 4};
	auto const tail = std::list{-5, -6, -7};
	head.append_range(tail);
	assert(std::ranges::equal(head, fast_io::vector<int>{1, 2, 3, 4, -5, -6, -7}));
}