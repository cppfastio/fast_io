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
		::fast_io::io::perr("Usage: ", fast_io::mnp::os_c_str(*argv), " <file> [creation_time (iso8601), now, omit] [last_modification_time (iso8601), now, omit] [last_access_time (iso8601), now, omit]\n");
		return 1;
	}

#if __cpp_exceptions
	try
#endif
	{

		::fast_io::unix_timestamp_option creation_time;
		::fast_io::unix_timestamp_option last_access_time;
		::fast_io::unix_timestamp_option last_modification_time;

		::fast_io::iso8601_timestamp iso8601_ts{};

		bool succussed{};
		try
		{
			iso8601_ts = ::fast_io::to<::fast_io::iso8601_timestamp>(fast_io::mnp::os_c_str(argv[2]));
			succussed = true;
		}
		catch (::fast_io::error)
		{
			succussed = false;
		}
		if (succussed)
		{
			creation_time.flags = ::fast_io::utime_flags::none;
			creation_time.timestamp = ::fast_io::to_timestamp(iso8601_ts);
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

		try
		{
			iso8601_ts = ::fast_io::to<::fast_io::iso8601_timestamp>(fast_io::mnp::os_c_str(argv[3]));
			succussed = true;
		}
		catch (::fast_io::error)
		{
			succussed = false;
		}
		if (succussed)
		{
			last_access_time.flags = ::fast_io::utime_flags::none;
			last_access_time.timestamp = ::fast_io::to_timestamp(iso8601_ts);
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

		try
		{
			iso8601_ts = ::fast_io::to<::fast_io::iso8601_timestamp>(fast_io::mnp::os_c_str(argv[4]));
			succussed = true;
		}
		catch (::fast_io::error)
		{
			succussed = false;
		}
		if (succussed)
		{
			last_modification_time.flags = ::fast_io::utime_flags::none;
			last_modification_time.timestamp = ::fast_io::to_timestamp(iso8601_ts);
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
