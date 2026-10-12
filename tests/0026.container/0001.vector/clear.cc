#include <string_view>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

void print_info(std::string_view rem, fast_io::vector<int> const &v)
{
	println(rem, "{ ", rgvw(v, " "), " }\nSize=", v.size(), ", Capacity=", v.capacity());
}

int main()
{
	fast_io::vector<int> container{1, 2, 3};
	print_info("Before clear: ", container);
	if (!(!container.is_empty())) ::fast_io::fast_terminate();
	if (!(container.size() == 3)) ::fast_io::fast_terminate();
	container.clear();
	print_info("After clear: ", container);
	if (!(container.is_empty())) ::fast_io::fast_terminate();
	if (!(container.size() == 0)) ::fast_io::fast_terminate();
}
