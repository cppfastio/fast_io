#include <cassert>
#include <iterator>
#include <algorithm>
#include <list>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	auto container = fast_io::vector{1, 2, 3, 4};
	auto pos = std::next(container.begin(), 2);
	assert(*pos == 3);
	auto const rg = std::list{-1, -2, -3};

	container.insert_range(pos, rg);
	assert(std::ranges::equal(container, fast_io::vector{1, 2, -1, -2, -3, 3, 4}));
}