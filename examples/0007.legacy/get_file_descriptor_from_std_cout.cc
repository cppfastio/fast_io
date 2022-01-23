#include<iostream>
#include<fast_io_legacy.h>

/*
C++ iostream is a holy crap. You cannot access FILE* inside std::cout because of the OO garbage.
See an old blog: https://www.ginac.de/~kreckel/fileno/

Also see wiki in fast_io to understand how C++ std::fstream work. It is implemented by FILE*.
So why do you use iostream?? It is just a garbage quality of RAII wrapper for FILE*.
https://github.com/expnkx/fast_io/wiki/0014.-How-does-std::fstream-work-internally%3F
*/

int main()
{
	fast_io::streambuf_io_observer siob{std::cout.rdbuf()};
//This is done by hacking C++ standard library implementation in fast_io.
// I hacked ALL c++ standard library implementation, including libstdc++, libc++ and MSVC stl
	println("std::cout.rdbuf():",siob.fb,"\n"
	"FILE*:",static_cast<fast_io::c_io_observer>(siob).fp,"\n"
	"fd:",static_cast<fast_io::posix_io_observer>(siob).fd
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
//On Windows we can also get its HANDLE
	,"\nwin32 HANDLE:",static_cast<fast_io::win32_io_observer>(siob).handle,"\n"
//Nt HANDLE and win32 HANDLE are the same
	"nt HANDLE:",static_cast<fast_io::nt_io_observer>(siob).handle,"\n"
	"zw HANDLE:",static_cast<fast_io::zw_io_observer>(siob).handle
//Zw and Nt are the same in user mode, but different in kernel mode. See Book: Windows Internal 7th version
//Or MSDN What Does the Zw Prefix Mean? https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/what-does-the-zw-prefix-mean
#endif
	);
}

/*
On Windows 10, it works like this
D:\hg\fast_io\examples\0007.legacy>get_file_descriptor_from_std_cout
std::cout.rdbuf():0x00007ff91879baa0
FILE*:0x00007ff95961fa30
fd:1
win32 HANDLE:0x0000000000000054
nt HANDLE:0x0000000000000054
zw HANDLE:0x0000000000000054
*/
