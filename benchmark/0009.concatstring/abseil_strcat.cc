#include <string>
#include <fast_io_dsal/string.h>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <absl/strings/str_cat.h>
using namespace fast_io::io;

int main()
{
	int test_i{};
	::std::string test_s("hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello\n");
	::std::string re;
	auto t0{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	for (std::size_t i{}; i != 1000000; ++i)
	{
		re = absl::StrCat("hello", test_i, test_s);
	}
	print(fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime) - t0, "s ", re);
}
