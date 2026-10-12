#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<char> letters{'a', 'b', 'c', 'd', 'e', 'f'};

	if (!letters.is_empty())
	{
		print("The last character is '", chvw(letters.back()), "'.\n");
	}
	if (!(letters.back() == 'f')) ::fast_io::fast_terminate();
}
