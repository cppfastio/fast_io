#include <fast_io_dsal/forward_list.h>
#include <fast_io_dsal/vector.h>
#include <fast_io.h>
#include <random>
#include <algorithm>

int main()
{
	::fast_io::vector<::std::size_t> vec;
	constexpr ::std::size_t n{10};
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
	::fast_io::forward_list<::std::size_t> flst2{1, 3, 5, 7, 9};
	::fast_io::io::println("flst:\n", ::fast_io::mnp::rgvw(flst, "\n"), "\nflst2:\n", ::fast_io::mnp::rgvw(flst2, "\n"));
}
