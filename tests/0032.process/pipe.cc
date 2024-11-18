#include <fast_io.h>
int main()
{
	try
	{
		fast_io::native_pipe pipe{};
		fast_io::native_process p{"test.exe", {}, {}, {{}, pipe, pipe}};
		char *ptr{(char *)_alloca(4096)};
		::fast_io::freestanding::bytes_clear_n((::std::byte *)ptr, 4096);
		char *last{};
		for (;;)
		{

			auto end = ::fast_io::operations::read_some(pipe.in(), ptr, ptr + 4096);
			if (end == last)
			{
				break;
			}
			last = end;
		}
		::fast_io::perrln("child: ", ::fast_io::mnp::os_c_str(ptr));
	}
	catch (fast_io::error err)
	{
		fast_io::io::perrln(err);
	}
}
