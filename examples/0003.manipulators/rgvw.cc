// rgvw is only available in host environment
#include <string>
#include <string_view>
#include <deque>
#include <set>
#include <unordered_set>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>

int main()
{
	using namespace ::fast_io::mnp;
	using namespace ::std::literals;
	fast_io::vector<int> i{ 1 };
	std::deque<int> d{ 1,2 };
	std::set<int> s{ 1,2,3 };
	std::unordered_set<int> u{ 1,2,3,4 };
	println(rgvw(i, " "));
	println(rgvw(d, fast_io::mnp::os_c_str(" ")));
	println(rgvw(s, "&"s));
	println(rgvw(u, " | "sv));
}
