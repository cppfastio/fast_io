#include<fast_io.h>
#include<fast_io_driver/llvm.h>

int main()
{
/*
This is an example to explain how fast_io's files work with each other, and how you could use fast_io with existing FILE* or fstream apis
*/
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#ifdef _WIN32_WINDOWS
//_WIN32_WINDOWS macro targets WIN9X kernel (windows 95, windows 98 and windows ME)
//On 9x kernel, although ntdll.dll does exist, it does not do anything.
	fast_io::win32_file nf("win32_file.txt",fast_io::open_mode::out);
#else
//NT kernel
	fast_io::nt_file nf("nt_file.txt",fast_io::open_mode::out);
#endif
	fast_io::posix_file pf(std::move(nf),fast_io::open_mode::out);
#else
	fast_io::posix_file pf("posix_file.txt",fast_io::open_mode::out);
#endif
	fast_io::llvm::raw_fd_ostream_file osf(std::move(pf),fast_io::open_mode::out);

	(*osf.os)<<"Hello World from llvm::raw_fd_ostream\n";
	print(osf,"Hello World from fast_io::llvm::raw_fd_ostream_file\n");
	fast_io::posix_tzset();
	auto unix_ts{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	println(
	"Unix Timestamp:",unix_ts,"\n"
	"Universe Timestamp:",static_cast<fast_io::universe_timestamp>(unix_ts),"\n"
	"UTC:",utc(unix_ts),"\n",
	"Local:",local(unix_ts)," Timezone:",fast_io::timezone_name(),"\n"
#ifdef __clang__
	"LLVM clang " __clang_version__ "\n"
#elif defined(__GNUC__) && defined(__VERSION__)
	"GCC " __VERSION__ "\n"
#elif defined(_MSC_VER)
	"Microsoft Visual C++ ",_MSC_VER,"\n"
#else
	"Unknown C++ compiler\n"
#endif
#if defined(_LIBCPP_VERSION)
	"LLVM libc++ ", _LIBCPP_VERSION, "\n"
#elif defined(__GLIBCXX__)
	"GCC libstdc++ ", __GLIBCXX__ , "\n"
#elif defined(_MSVC_STL_UPDATE)
	"Microsoft Visual C++ STL ", _MSVC_STL_UPDATE, "\n"
#else
	"Unknown C++ standard library\n"
#endif
	"LLVM raw_fd_stream address:",osf.os,"\n"
	"fd:",static_cast<fast_io::posix_io_observer>(osf).fd
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	,"\n"
	"win32 HANDLE:",static_cast<fast_io::win32_io_observer>(osf).handle
#ifndef _WIN32_WINDOWS
//NT kernel
	,"\n"
	"zw HANDLE:",static_cast<fast_io::zw_io_observer>(osf).handle,"\n"
	"nt HANDLE:",static_cast<fast_io::nt_io_observer>(osf).handle
#endif
#endif
);
}
/*
MinGW-w64 needs -lntdll
g++ -o construct_fstream_from_syscall construct_fstream_from_syscall.cc -Ofast -std=c++20 -s -flto -march=native -lntdll

9x kernel with clang might need
-Wl,--major-subsystem-version -Wl,4 -Wl,--minor-subsystem-version -Wl,0 -D_WIN32_WINDOWS=0x0400 -DWINVER=0x0400

We also need to link to libLLVM-xxyyy.so or libLLVM-xxyyy.dll
*/
