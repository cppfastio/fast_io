#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<::std::size_t> vec;
	println("Before vec.push_back(50): vec.size()=",vec.size()," vec.capacity()=",vec.capacity());
	vec.push_back(50);
	vec.push_back(50);
	println("After vec.push_back(50): vec.size()=",vec.size()," vec.capacity()=",vec.capacity());
}