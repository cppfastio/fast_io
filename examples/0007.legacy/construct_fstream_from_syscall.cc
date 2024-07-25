﻿#include <fast_io_legacy.h>
#include <fstream>

using namespace fast_io::io;

int main()
{
/*
This is an example to explain how fast_io's files work with each other, and how you could use fast_io with existing
FILE* or fstream apis
*/
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#ifdef _WIN32_WINDOWS
	//_WIN32_WINDOWS macro targets WIN9X kernel (windows 95, windows 98 and windows ME)
	// On 9x kernel, although ntdll.dll does exist, it does not do anything.
	fast_io::win32_file nf("win32_file.txt", fast_io::open_mode::out);
#else
	// NT kernel
	fast_io::nt_file nf("nt_file.txt", fast_io::open_mode::out);
#endif
	fast_io::posix_file pf(std::move(nf), fast_io::open_mode::out);
#else
	fast_io::posix_file pf("posix_file.txt", fast_io::open_mode::out);
#endif
	fast_io::c_file cf(std::move(pf), fast_io::open_mode::out);
	fast_io::filebuf_file fbf(std::move(cf), fast_io::open_mode::out);
	std::ofstream fout;
	*fout.rdbuf() = std::move(*fbf.fb);
	fast_io::filebuf_io_observer fiob{fout.rdbuf()};
	fout << "Hello World from std::ofstream\n";
	print(fiob, "Hello World from fast_io::filebuf_io_observer\n");
	fast_io::posix_tzset();
	auto unix_ts{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	println("Unix Timestamp:", unix_ts,
			"\n"
			"Universe Timestamp:",
			static_cast<fast_io::universe_timestamp>(unix_ts),
			"\n"
			"UTC:",
			utc(unix_ts), "\n", "Local:", local(unix_ts), " Timezone:", fast_io::timezone_name(),
			"\n"
#ifdef __clang__
			"LLVM clang " __clang_version__ "\n"
#elif defined(__GNUC__) && defined(__VERSION__)
			"GCC " __VERSION__ "\n"
#elif defined(_MSC_VER)
			"Microsoft Visual C++ ",
			_MSC_VER,
			"\n"
#else
			"Unknown C++ compiler\n"
#endif
#if defined(__GLIBC__)
			"GNU C Library ",
			__GLIBC__, ".", __GLIBC_MINOR__,
#ifdef __GLIBC_SOURCE__
			" (Source: ",__GLIBC_SOURCE__, ".", __GLIBC_MINOR_SOURCE__,")"
#endif
			"\n"
#elif defined(__CYGWIN__) || defined(__NEWLIB__)
			"Newlib cygwin\n"
#elif defined(_WIN32) && !defined(__WINE__)
#if defined(_UCRT) || defined(_MSC_VER)
			"Universal CRT\n"
#elif defined(__MSVCRT_VERSION__)
			"MSVCRT ",
			fast_io::mnp::hex0x(__MSVCRT_VERSION__),
			"\n"
#endif
#endif
#if defined(_LIBCPP_VERSION)
			"LLVM libc++ ",
			_LIBCPP_VERSION,
			"\n"
#elif defined(__GLIBCXX__)
			"GNU C++ Library ",
			_GLIBCXX_RELEASE, " ", __GLIBCXX__,
			"\n"
#elif defined(_MSVC_STL_UPDATE)
			"Microsoft Visual C++ STL ",
			_MSVC_STL_UPDATE,
			"\n"
#else
			"Unknown C++ standard library\n"
#endif
			"fstream.rdbuf():",
			::fast_io::mnp::handlevw(fiob.fb),
			"\n"
			"FILE*:",
			::fast_io::mnp::handlevw(static_cast<fast_io::c_io_observer>(fiob).fp),
			"\n"
			"fd:",
			static_cast<fast_io::posix_io_observer>(fiob).fd
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
			,
			"\n"
			"win32 HANDLE:",
			::fast_io::mnp::handlevw(static_cast<fast_io::win32_io_observer>(fiob).handle)
#ifndef _WIN32_WINDOWS
			// NT kernel
			,
			"\n"
			"zw HANDLE:",
			::fast_io::mnp::handlevw(static_cast<fast_io::zw_io_observer>(fiob).handle),
			"\n"
			"nt HANDLE:",
			::fast_io::mnp::handlevw(static_cast<fast_io::nt_io_observer>(fiob).handle)
#endif
#endif
	);
}
/*
MinGW-w64 needs -lntdll
g++ -o construct_fstream_from_syscall construct_fstream_from_syscall.cc -Ofast -std=c++20 -s -flto -march=native -lntdll

9x kernel with clang might need
-Wl,--major-subsystem-version -Wl,4 -Wl,--minor-subsystem-version -Wl,0 -D_WIN32_WINDOWS=0x0400 -DWINVER=0x0400

D:\hg\fast_io\examples\0007.legacy>g++ -o construct_fstream_from_syscall construct_fstream_from_syscall.cc -Ofast
-std=c++23 -s -flto -march=native -I../../include -lntdll

D:\hg\fast_io\examples\0007.legacy>construct_fstream_from_syscall
Unix Timestamp:1663581526.3485249
Universe Timestamp:434602343092816726.3485249
UTC:2022-09-19T09:58:46.3485249Z
Local:2022-09-19T05:58:46.3485249-04:00 Timezone:Eastern Daylight Time
GCC 13.0.0 20220703 (experimental)
MSVCRT 0x700
GNU C++ Library 13 20220703
fstream.rdbuf():0x0000004a1f3ffb68
FILE*:0x00007ff92eb5fa90
fd:3
win32 HANDLE:0x00000000000000a8
zw HANDLE:0x00000000000000a8
nt HANDLE:0x00000000000000a8
*/
