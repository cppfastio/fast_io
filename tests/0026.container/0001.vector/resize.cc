#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

void print_info(auto rem, fast_io::vector<int> const &c)
{
	println(os_c_str(rem), rgvw(c, " "));
}

int main()
{
	fast_io::vector<int> c = {1, 2, 3};
	print_info("The vector holds: ", c);

	c.resize(5);
	print_info("After resize up to 5: ", c);

	c.resize(2);
	print_info("After resize down to 2: ", c);

	c.resize(6, 4);
	print_info("After resize up to 6 (initializer = 4): ", c);
}