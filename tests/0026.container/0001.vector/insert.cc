#include <iterator>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

void print_info(int id, fast_io::vector<int> const &container)
{
	perrln(id, ". ", rgvw(container, " "));
}

int main()
{
	fast_io::vector<int> c1(3, 100);
	print_info(1, c1);

	auto it = c1.begin();
	it = c1.insert(it, 200);
	print_info(2, c1);

	c1.insert(it, 2, 300);
	print_info(3, c1);

	// `it` no longer valid, get a new one:
	it = c1.begin();

	fast_io::vector<int> c2(2, 400);
	c1.insert(std::next(it, 2), c2.begin(), c2.end());
	print_info(4, c1);

	int arr[] = {501, 502, 503};
	c1.insert(c1.begin(), arr, arr + std::size(arr));
	print_info(5, c1);

	c1.insert(c1.end(), {601, 602, 603});
	print_info(6, c1);
}