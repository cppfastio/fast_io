#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	/*
	Typically, file stream functionalities are not implemented at the OS native API level.
	In Microsoft DOS, LF to CRLF conversion is carried out at the POSIX level using two systems.
	However, in Windows, LF to CRLF conversion occurs at the file descriptor level, though not at the Win32 or NT level; rather, it involves CRT tricks.
	It's likely that other operating systems implement this conversion at the FILE* level as well.

	Utilizing fast_io's ibuf_file or obuf_file for LF to CRLF conversion lacks both portability and performance.
	Instead, it's preferable to leverage the existing FILE* facilities.
	Concerns regarding performance are unnecessary; I've modified the FILE* implementation across all libc implementations.
	Note that c_file_unlocked does not lock FILE*.
	*/
	fast_io::c_file_unlocked cfl("text.txt", fast_io::open_mode::out |
												 fast_io::open_mode::text); // add open_mode::text to open_mode flag
	fast_io::posix_tzset();
	auto unix_ts{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	using namespace fast_io::mnp;
	print(cfl, "Unix Timestamp:", unix_ts,
		  "\nUTC:", utc(unix_ts),
		  "\nLocal:", local(unix_ts), " Timezone:", fast_io::timezone_name());
#ifdef __clang__
	print(cfl, "\nLLVM clang " __clang_version__);
#elif defined(__GNUC__)
	print(cfl, "\ngcc ", __GNUC__);
#elif defined(_MSC_VER)
	print(cfl, "\nMicrosoft Visual C++ ", _MSC_VER);
#else
	print(cfl, "\nUnknown C++ compiler");
#endif
#if defined(__GLIBCXX__)
	print(cfl, "\nGCC libstdc++ ", __GLIBCXX__);
#elif defined(_LIBCPP_VERSION)
	print(cfl, "\nLLVM libc++ " _LIBCPP_VERSION);
#elif defined(_MSVC_STL_UPDATE)
	print(cfl, "\nMicrosoft Visual C++ STL ", _MSVC_STL_UPDATE);
#else
	print(cfl, "\nUnknown C++ standard library");
#endif
	println(cfl,
			"\nFILE*:", handlevw(cfl.fp),
			"\nfd:", handlevw(static_cast<fast_io::posix_io_observer>(cfl).fd)
#ifdef _WIN32
						 ,
			"\nwin32 HANDLE:", handlevw(static_cast<fast_io::win32_io_observer>(cfl).handle),
			"\nzw HANDLE:", handlevw(static_cast<fast_io::zw_io_observer>(cfl).handle),
			"\nnt HANDLE:", handlevw(static_cast<fast_io::nt_io_observer>(cfl).handle)
#endif
	);
}
/*
Demo output:
Unix Timestamp:1620354611.4156443
Universe Timestamp:434602343049589811.4156443
UTC:2021-05-07T02:30:11.4156443Z
Local:2021-05-06T22:30:11.4156443-04:00 Timezone:Eastern Daylight Time
gcc 12
GCC libstdc++ 20210505
FILE*:0x00007ff95961fa90
fd:3
win32 HANDLE:0x00000000000000b8
zw HANDLE:0x00000000000000b8
nt HANDLE:0x00000000000000b8
*/