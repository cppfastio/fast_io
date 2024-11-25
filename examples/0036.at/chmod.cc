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
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> <perms>\n");
		return 1;
	}

#if __cpp_exceptions
	try
#endif
	{
		::std::uint_least32_t perm;
		::fast_io::scan(::fast_io::mnp::os_c_str(argv[2]), perm);
		::fast_io::native_fchmodat(::fast_io::at_fdcwd(), fast_io::mnp::os_c_str(argv[1]), static_cast<::fast_io::perms>(perm));
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
