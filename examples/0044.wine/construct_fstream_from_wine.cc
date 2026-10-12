#include <fast_io.h>
#include <fast_io_legacy.h>
#include <fstream>

/*
wine_file -> std::ofstream, like construct_fstream_from_syscall.cc.

filebuf_file move-constructs straight from wine_file: the chain goes
wine_file -> nt_file -> posix_file -> c_file -> filebuf_file, with the
bottom hop handing the released host_fd to __wine_unix_host_fd_to_nt_handle,
which transfers ownership — a wineserver-registered HANDLE on the unixcall
impl, the same HANDLE bits on the nt impl.
*/

int main()
try
{
	::fast_io::wine_file wf(u8"wine_file.txt", ::fast_io::open_mode::out);
	::fast_io::filebuf_file fbf(::std::move(wf), ::fast_io::open_mode::out);
	::std::ofstream fout;
	*fout.rdbuf() = ::std::move(*fbf.fb);
	::fast_io::filebuf_io_observer fiob{fout.rdbuf()};
	fout << "Hello World from std::ofstream\n";
	::fast_io::print(fiob, "Hello World from fast_io::filebuf_io_observer\n");
	fast_io::posix_tzset();
	auto unix_ts{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	println("Unix Timestamp:", unix_ts,
			"\n"
			"UTC:",
			utc(unix_ts), "\n"
						  "Local:",
			local(unix_ts), " Timezone:", fast_io::timezone_name(),
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
			" (Source: ", __GLIBC_SOURCE__, ".", __GLIBC_MINOR_SOURCE__, ")"
#endif
																		 "\n"
#elif defined(__BIONIC__)
			"Bionic\n"
#elif defined(__CYGWIN__) || defined(__NEWLIB__)
			"Newlib cygwin\n"
#elif defined(_WIN32) && !defined(__WINE__)
#if defined(_UCRT) || defined(_MSC_VER)
			"Universal CRT"
#if defined(__MINGW32__)
			"(GNU)"
#endif
			"\n"
#elif defined(__MSVCRT_VERSION__)
			"MSVCRT ",
			fast_io::mnp::hex0x(__MSVCRT_VERSION__),
			"\n"
#endif
#elif defined(__DARWIN_C_LEVEL)
			"Darwin ",
			__DARWIN_C_LEVEL, "\n"
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
			::fast_io::mnp::handlevw(static_cast<fast_io::nt_io_observer>(fiob).handle),
			"\n"
			"wine host_fd:",
			::fast_io::mnp::handlevw(static_cast<fast_io::wine_io_observer>(fiob).host_fd)
#endif
#endif
	);
}
catch throws(::std::error e)
{
	::fast_io::perrln(e);
	return 1;
}
