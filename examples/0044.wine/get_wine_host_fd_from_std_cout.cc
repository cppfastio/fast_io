#include <iostream>
#include <fast_io_legacy.h>

int main()
{
	::fast_io::streambuf_io_observer siob{std::cout.rdbuf()};
	::fast_io::wine_io_observer wniob{static_cast<::fast_io::wine_io_observer>(siob)};
	using namespace fast_io::iomnp;
	println(wniob, "std::cout.rdbuf():", handlevw(siob.fb),
			"\n"
			"FILE*:",
			handlevw(static_cast<fast_io::c_io_observer>(siob).fp),
			"\n"
			"fd:",
			handlevw(static_cast<fast_io::posix_io_observer>(siob).fd)
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
			// On Windows we can also get its HANDLE
			,
			"\nwin32 HANDLE:", handlevw(static_cast<fast_io::win32_io_observer>(siob).handle),
			"\n"
			// Nt HANDLE and win32 HANDLE are the same
			"nt HANDLE:",
			handlevw(static_cast<fast_io::nt_io_observer>(siob).handle),
			"\n"
			"zw HANDLE:",
			handlevw(static_cast<fast_io::zw_io_observer>(siob).handle),
			"\n"
			"wine host_fd:",
			handlevw(wniob.host_fd)
#endif
	);
}

/*
$ clang++ -o get_wine_host_fd_from_std_cout.exe get_wine_host_fd_from_std_cout.cc -O3 --config=$HOME/herbcfgs/x86_64-windows-msvc.cfg -fherbceptions -lherbceptions -flto=thin -L$HOME/libraries/fast_io_kilo/winelibc_wine_build -lwineunix
$ WINEDLLPATH=$HOME/libraries/fast_io_kilo/winelibc_wine_build WINEPATH="$HOME/libraries/fast_io_kilo/winelibc_wine_build;$WINEPATH" wine ./get_wine_host_fd_from_std_cout.exe
std::cout.rdbuf():0x00006ffff6c5eb40
FILE*:0x00006ffffea12278
fd:1
win32 HANDLE:0x0000000000000010
nt HANDLE:0x0000000000000010
zw HANDLE:0x0000000000000010
wine host_fd:2
*/
