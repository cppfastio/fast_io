#include <fast_io.h>
#include <fast_io_dsal/string.h>
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

#ifdef _WIN32
	// ::fast_io::default_args : Same parameters as the corresponding system API
	[[maybe_unused]] ::fast_io::native_process_args native_args{::fast_io::default_args, u""};
#endif

	::fast_io::native_process p{
		::fast_io::mnp::os_c_str(argv[1]),
		{"char",
		 L"wchar Double\"quotation\"marks",
		 u8"u8",
		 u"u16",
		 U"u32",
		 114514,
		 115145.1919810,
		 ::fast_io::concat_fast_io("This ", 1, "s ", ::fast_io::mnp::code_cvt_os_c_str(u8"just "), "a parameter")},
		{"env1",
		 L"env2",
		 3,
		 4.0},
		{.in = ::fast_io::posix_dev_null(), .out = ::fast_io::out(), .err = ::fast_io::err()},
		::fast_io::process_mode::none};
	auto ec{wait(p)};
	::fast_io::io::perrln(::fast_io::mnp::os_c_str(*argv), " -> Exit code: ", static_cast<int>(ec.wait_loc));
}
