#include <fast_io.h>

int main()
{
	for (::std::size_t i = 0; i < 100; i++)
	{
		::fast_io::io::perrln(::fast_io::out(), i, ": helloworld from out\n");
		::fast_io::io::perrln(::fast_io::err(), i, ": helloworld from err\n");
		_sleep(1000);

	}
}
