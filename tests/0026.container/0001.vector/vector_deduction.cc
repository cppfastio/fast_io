#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector vec{static_cast<::std::uint_least32_t>(1), 6, 10, 20};
	println("Before vec.push_back(50): vec.size()=", vec.size(), " vec.capacity()=", vec.capacity());
	vec.push_back(50);
	vec.push_back(50);
	println("After vec.push_back(50): vec.size()=", vec.size(), " vec.capacity()=", vec.capacity());
}