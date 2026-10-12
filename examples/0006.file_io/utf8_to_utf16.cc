#include <fast_io.h>
#include <fast_io_device.h>

using namespace fast_io::io;

int main(int argc, char **argv)
#ifdef __cpp_exceptions
try
#endif
{
	using namespace fast_io::mnp;
	if (argc != 3)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", os_c_str(*argv), " <input UTF8 file> <output UTF-32 file>\n");
		return 1;
	}
	auto t0{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	fast_io::u16iutf8_file file(os_c_str(argv[1])); // This will do codecvt from UTF-8 to UTF-16
	fast_io::obuf_file obf(os_c_str(argv[2]), fast_io::open_mode::excl);
	::std::size_t transmitted{};
	for (::std::size_t n{}; (n = ::fast_io::operations::transmit_some_bytes(
								   obf, {}, file, {}, ::std::numeric_limits<::std::size_t>::max())) != 0;)
	{
		transmitted += n;
	}
	println(os_c_str(argv[1]), " => ", os_c_str(argv[2]), "\nTransmitted:", transmitted,
			" utf-16 bytes\tElapsed Time:", fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime) - t0);
}
#if defined(__HERBCEPTIONS__)
catch throws(::std::error e)
{
	perrln(e);
}
#endif
#ifdef __cpp_exceptions
catch (std::exception const &e)
{
	perrln(fast_io::mnp::ehvw(e));
}
catch (...)
{
	perr("unknown EH\n");
}
#endif
