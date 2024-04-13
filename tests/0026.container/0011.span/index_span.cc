#include <fast_io_dsal/array.h>
#include <fast_io_dsal/index_span.h>
#include <fast_io.h>

int main()
{
	::fast_io::array<::std::size_t, 40> arr{4, 6, 7};
	::fast_io::index_span<::std::size_t, 5> sp(arr);
	for (auto const &e : sp)
	{
		::fast_io::io::println(e);
	}
}