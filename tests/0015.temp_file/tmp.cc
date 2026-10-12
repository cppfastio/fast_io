#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	fast_io::native_file file(fast_io::io_temp);
	for (std::size_t i{}; i != 1000; ++i)
	{
		print(file, "Hello World\n");
	}
	rewind(file);
	::std::size_t transmitted{};
	for (::std::size_t n{}; (n = ::fast_io::operations::transmit_some_bytes(
								   fast_io::c_stdout(), {}, file, {},
								   ::std::numeric_limits<::std::size_t>::max())) != 0;)
	{
		transmitted += n;
	}
	println("transmitted:", transmitted);
}
