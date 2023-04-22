#include <string>
#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	auto tsp1 = fast_io::to<fast_io::win32_timestamp>("1231232.23423423423423423435");
	auto tsp2 = fast_io::to<fast_io::win32_timestamp>("1", "2", "3", "1", "2", "3", "2", ".", "2", "3", "4", "2", "3", "4", "2", "3", "4", "2", "3", "4", "2", "3", "4", "2", "3", "4", "3", "5");
	println("tsp1 == tsp2: ", fast_io::mnp::boolalpha(tsp1 == tsp2));
	auto s = fast_io::concat(tsp1);
	println("is scan correct: ", fast_io::mnp::boolalpha(s == "11645704832.2342342342342342344"));
}