#include<fast_io.h>
#include<fast_io_driver/python/pyobject.h>

using namespace fast_io::io;

int main()
{
	auto obj{fast_io::concat_pyobject_file("Hello World\n")};
	print(obj);
}

