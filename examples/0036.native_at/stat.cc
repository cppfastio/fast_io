#include <fast_io.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file>\n");
		return 1;
	}

#if __cpp_exceptions
	try
#endif
	{
		::fast_io::native_file f{fast_io::mnp::os_c_str(argv[1]), ::fast_io::open_mode::in};
		::fast_io::io::perrln(status(f));
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
