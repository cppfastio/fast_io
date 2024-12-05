#include <fast_io.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		if (argc == 0) [[unlikely]]
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <exe>\n");
		return 1;
	}
	::fast_io::native_process p{::fast_io::mnp::os_c_str(argv[1]), {}, {}, {.in = fast_io::in(), .out = fast_io::out(), .err = fast_io::err()}};
	auto ec{wait(p)};
	::fast_io::io::perrln(::fast_io::mnp::os_c_str(*argv), " -> Exit code: ", static_cast<int>(ec.wait_loc));
}
