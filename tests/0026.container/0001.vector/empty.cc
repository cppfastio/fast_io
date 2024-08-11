#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> numbers;
	println("Initially, numbers.empty(): ", boolalpha(numbers.empty()));

	numbers.push_back(42);
	println("After adding elements, numbers.empty(): ", boolalpha(numbers.empty()));
}