#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> v;
	println("Default-constructed capacity is ", v.capacity());
	v.resize(100);
	println("Capacity of a 100-element vector is ", v.capacity());
	v.resize(50);
	println("Capacity after resize(50) is ", v.capacity());
	v.shrink_to_fit();
	println("Capacity after shrink_to_fit() is ", v.capacity());
	v.clear();
	println("Capacity after clear() is ", v.capacity());
	v.shrink_to_fit();
	println("Capacity after shrink_to_fit() is ", v.capacity());
	for (int i = 1000; i < 1300; ++i)
	{
		v.push_back(i);
	}
	println("Capacity after adding 300 elements is ", v.capacity());
	v.shrink_to_fit();
	println("Capacity after shrink_to_fit() is ", v.capacity());
}