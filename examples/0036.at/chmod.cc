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
		auto const [p1, e1] = ::fast_io::parse_by_scan(argv[2], argv[2] + ::fast_io::cstr_len(argv[2]), ::fast_io::mnp::base_get<8>(perm));
		if (e1 != ::fast_io::parse_code::ok)
		{
			::fast_io::throw_parse_code(e1);
		}

		::fast_io::native_fchmodat(::fast_io::at_fdcwd(), fast_io::mnp::os_c_str(argv[1]), static_cast<::fast_io::perms>(perm));
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
