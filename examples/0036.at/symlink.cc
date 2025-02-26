#include <fast_io.h>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <oldfile> <newfile>\n");
		return 1;
	}

#if defined(_WIN32)
	void* state{nullptr};
	auto privilege{fast_io::win32::nt::privileges::SE_CREATE_SYMBOLIC_LINK_PRIVILEGE};

    if (fast_io::win32::nt::RtlAcquirePrivilege(__builtin_addressof(privilege), 1, 0, __builtin_addressof(state))) [[unlikely]]
	{
        fast_io::print(fast_io::c_stderr(), "RtlAcquirePrivilege failed with status\n");
		return 1;
	}
#endif  // defined(_WIN32)

#if __cpp_exceptions
	try
#endif
	{
		::fast_io::native_symlinkat(fast_io::mnp::os_c_str(argv[1]),
									::fast_io::at_fdcwd(), fast_io::mnp::os_c_str(argv[2]));
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
