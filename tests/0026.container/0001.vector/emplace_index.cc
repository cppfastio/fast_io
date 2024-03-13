#include <algorithm>
#include <ranges>
#include <string>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::vector<::std::size_t> vec;
	vec.push_back(4);
	vec.emplace_index(0,6);
	vec.emplace_index(0,8);
	for(auto const & e : vec)
	{
		::fast_io::io::println(e);
	}
}
