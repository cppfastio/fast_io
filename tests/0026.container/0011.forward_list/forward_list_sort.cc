#include <fast_io_dsal/forward_list.h>
#include <fast_io_dsal/vector.h>
#include <fast_io.h>
#include <random>
#include <algorithm>

int main()
{
	::fast_io::forward_list<::std::size_t> vec;
	constexpr ::std::size_t n{100000};
	vec.reserve(n);
	for (::std::size_t i{}; i != n; ++i)
	{
		vec.push_back_unchecked(i);
	}
	::fast_io::ibuf_white_hole_engine eng;
	::std::ranges::shuffle(vec, eng);

	::fast_io::forward_list<::std::size_t> flst(::fast_io::freestanding::from_range, vec);
#if 0
	flst.sort();
#endif
	::fast_io::io::print(::fast_io::mnp::rgvw(flst,"\n"));
}
