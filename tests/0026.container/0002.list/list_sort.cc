#include <fast_io_dsal/list.h>
#include <fast_io_dsal/vector.h>
#include <fast_io.h>
#include <random>
#include <algorithm>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::vector<::std::size_t> vec;
	constexpr
		::std::size_t n{10};
	vec.reserve(n);
	for(::std::size_t i{};i!=n;++i)
	{
		vec.push_back_unchecked(i);
	}
	::fast_io::ibuf_white_hole_engine eng;
	::std::ranges::shuffle(vec,eng);

	::fast_io::list<::std::size_t> lst(vec.begin(),vec.end());


	lst.sort();
#if 1
	for(auto const & e : lst)
	{
		println(e);
	}
#endif
}