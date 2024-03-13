#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

inline void print_container(fast_io::vector<int> const &c)
{
	println(rgvw(c, " "));
}

int main()
{
	fast_io::vector<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	print_container(c);

	c.erase_index(0);
	print_container(c);

	c.erase_index(2, 5);
	print_container(c);

	// Erase all even numbers
	for (::std::size_t i{}; i!=c.size();)
	{
		if (c[i] % 2 == 0)
		{
			c.erase_index(i);
		}
		else
		{
			++i;
		}
	}
	print_container(c);
}