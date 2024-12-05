#include <fast_io.h>
#include <fast_io_dsal/string_view.h>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [f, x, w, r]\n");
		return 1;
	}

#if __cpp_exceptions
	try
#endif
	{
		::fast_io::u8cstring_view argv2{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[2]))};
		::fast_io::access_how ah;
		if (argv2 == u8"f")
		{
			ah = ::fast_io::access_how::f_ok;
		}
		else if (argv2 == u8"x")
		{
			ah = ::fast_io::access_how::x_ok;
		}
		else if (argv2 == u8"w")
		{
			ah = ::fast_io::access_how::w_ok;
		}
		else if (argv2 == u8"r")
		{
			ah = ::fast_io::access_how::r_ok;
		}
		else
		{
			::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [f, x, w, r]\n");
			return 1;
		}

		::fast_io::native_faccessat(::fast_io::at_fdcwd(), fast_io::mnp::os_c_str(argv[1]), ah);
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
