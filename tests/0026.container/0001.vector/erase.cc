#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

void print_container(fast_io::vector<int> const &c)
{
	println(rgvw(c, " "));
}

int main()
{
	fast_io::vector<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	print_container(c);

	c.erase(c.begin());
	print_container(c);

	c.erase(c.begin() + 2, c.begin() + 5);
	print_container(c);

	// Erase all even numbers
	for (fast_io::vector<int>::iterator it = c.begin(); it != c.end();)
	{
		if (*it % 2 == 0)
		{
			it = c.erase(it);
		}
		else
		{
			++it;
		}
	}
	print_container(c);
}