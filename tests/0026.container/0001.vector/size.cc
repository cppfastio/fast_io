#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> nums{1, 3, 5, 7};

	print("nums contains ", nums.size(), " elements.\n");
}