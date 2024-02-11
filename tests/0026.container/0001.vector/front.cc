#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<char> letters{'a', 'b', 'c', 'd', 'e', 'f'};

	if (!letters.empty())
	{
		print("The first character is '", chvw(letters.front()), "'.\n");
	}
}