#include <fast_io.h>
#include <fast_io_dsal/string_view.h>

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [creation_time, now, omit] [last_modification_time, now, omit] [last_access_time, now, omit]\n");
		return 1;
	}

#if __cpp_exceptions
	try
#endif
	{

		::fast_io::unix_timestamp_option creation_time;
		::fast_io::unix_timestamp_option last_access_time;
		::fast_io::unix_timestamp_option last_modification_time;

		if (::fast_io::scan<true>(fast_io::mnp::os_c_str(argv[2]), creation_time.timestamp))
		{
			creation_time.flags = ::fast_io::utime_flags::none;
		}
		else if (::fast_io::u8cstring_view{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[2]))} == u8"now")
		{
			creation_time.flags = ::fast_io::utime_flags::now;
		}
		else if (::fast_io::u8cstring_view{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[2]))} == u8"omit")
		{
			creation_time.flags = ::fast_io::utime_flags::omit;
		}
		else
		{
			::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [creation_time, now, omit] [last_modification_time, now, omit] [last_access_time, now, omit]\n");
			return 1;
		}

		if (::fast_io::scan<true>(fast_io::mnp::os_c_str(argv[3]), last_access_time.timestamp))
		{
			last_access_time.flags = ::fast_io::utime_flags::none;
		}
		else if (::fast_io::u8cstring_view{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[3]))} == u8"now")
		{
			last_access_time.flags = ::fast_io::utime_flags::now;
		}
		else if (::fast_io::u8cstring_view{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[3]))} == u8"omit")
		{
			last_access_time.flags = ::fast_io::utime_flags::omit;
		}
		else
		{
			::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [creation_time, now, omit] [last_modification_time, now, omit] [last_access_time, now, omit]\n");
			return 1;
		}

		if (::fast_io::scan<true>(fast_io::mnp::os_c_str(argv[4]), last_modification_time.timestamp))
		{
			last_modification_time.flags = ::fast_io::utime_flags::none;
		}
		else if (::fast_io::u8cstring_view{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[4]))} == u8"now")
		{
			last_modification_time.flags = ::fast_io::utime_flags::now;
		}
		else if (::fast_io::u8cstring_view{fast_io::mnp::os_c_str(reinterpret_cast<char8_t *>(argv[4]))} == u8"omit")
		{
			last_modification_time.flags = ::fast_io::utime_flags::omit;
		}
		else
		{
			::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [creation_time, now, omit] [last_modification_time, now, omit] [last_access_time, now, omit]\n");
			return 1;
		}

		::fast_io::native_utimensat(::fast_io::at_fdcwd(), fast_io::mnp::os_c_str(argv[1]), creation_time, last_access_time, last_modification_time);
	}
#if __cpp_exceptions
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
#endif
}
