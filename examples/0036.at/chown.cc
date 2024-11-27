#include <fast_io.h>
#include <fast_io_dsal/string_view.h>

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> <owner> <group>\n");
		return 1;
	}

#if __cpp_exceptions
	try
#endif
	{
		::std::uint_least64_t o;
		::std::uint_least64_t g;

		::fast_io::ibuffer_view ibv{argv[2], argv[2] + ::fast_io::cstr_len(argv[2])};
		::fast_io::scan(ibv, ::fast_io::mnp::hex_get<8>(o), ::fast_io::mnp::hex_get<8>(g));
		::fast_io::native_fchownat(::fast_io::at_fdcwd(), fast_io::mnp::os_c_str(argv[1]), o, g);
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
