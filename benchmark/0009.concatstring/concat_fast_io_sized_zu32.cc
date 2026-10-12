#include <fast_io_dsal/sized_string.h>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	int test_i{};
	::fast_io::sized_string_zu32 test_s("hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello\n");
	::fast_io::sized_string_zu32 re;
	auto t0{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	for (std::size_t i{}; i != 1000000; ++i)
	{
		re = fast_io::concat_fast_io_sized_zu32("hello", test_i, test_s);
	}
	print(fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime) - t0, "s ", re);
}
