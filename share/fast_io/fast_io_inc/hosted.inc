export namespace fast_io
{

using ::fast_io::fast_terminate;

#if defined(_WIN32) || defined(__CYGWIN__)
using ::fast_io::win32_family;
using ::fast_io::nt_family;

using ::fast_io::basic_nt_family_io_observer;
using ::fast_io::basic_nt_family_file;

using ::fast_io::basic_nt_io_observer;
using ::fast_io::basic_nt_file;

using ::fast_io::nt_io_observer;
using ::fast_io::nt_file;
using ::fast_io::wnt_io_observer;
using ::fast_io::wnt_file;
using ::fast_io::u8nt_io_observer;
using ::fast_io::u8nt_file;
using ::fast_io::u16nt_io_observer;
using ::fast_io::u16nt_file;
using ::fast_io::u32nt_io_observer;
using ::fast_io::u32nt_file;

using ::fast_io::nt_stdin;
using ::fast_io::nt_stdout;
using ::fast_io::nt_stderr;

using ::fast_io::basic_zw_io_observer;
using ::fast_io::basic_zw_file;

using ::fast_io::zw_io_observer;
using ::fast_io::zw_file;
using ::fast_io::wzw_io_observer;
using ::fast_io::wzw_file;
using ::fast_io::u8zw_io_observer;
using ::fast_io::u8zw_file;
using ::fast_io::u16zw_io_observer;
using ::fast_io::u16zw_file;
using ::fast_io::u32zw_io_observer;
using ::fast_io::u32zw_file;

using ::fast_io::zw_stdin;
using ::fast_io::zw_stdout;
using ::fast_io::zw_stderr;

using ::fast_io::basic_win32_family_io_observer;
using ::fast_io::basic_win32_family_file;

using ::fast_io::basic_win32_io_observer;
using ::fast_io::basic_win32_file;

using ::fast_io::win32_io_observer;
using ::fast_io::win32_file;
using ::fast_io::wwin32_io_observer;
using ::fast_io::wwin32_file;
using ::fast_io::u8win32_io_observer;
using ::fast_io::u8win32_file;
using ::fast_io::u16win32_io_observer;
using ::fast_io::u16win32_file;
using ::fast_io::u32win32_io_observer;
using ::fast_io::u32win32_file;

using ::fast_io::win32_stdin;
using ::fast_io::win32_stdout;
using ::fast_io::win32_stderr;

#endif

using ::fast_io::basic_posix_family_io_observer;
using ::fast_io::basic_posix_family_file;

using ::fast_io::posix_io_observer;
using ::fast_io::posix_file;
using ::fast_io::wposix_io_observer;
using ::fast_io::wposix_file;
using ::fast_io::u8posix_io_observer;
using ::fast_io::u8posix_file;
using ::fast_io::u16posix_io_observer;
using ::fast_io::u16posix_file;
using ::fast_io::u32posix_io_observer;
using ::fast_io::u32posix_file;

using ::fast_io::native_file;
using ::fast_io::native_io_observer;
using ::fast_io::wnative_file;
using ::fast_io::wnative_io_observer;
using ::fast_io::u8native_file;
using ::fast_io::u8native_io_observer;
using ::fast_io::u16native_file;
using ::fast_io::u16native_io_observer;
using ::fast_io::u32native_file;
using ::fast_io::u32native_io_observer;

using ::fast_io::at;
using ::fast_io::drt;
using ::fast_io::current;
using ::fast_io::recursive;

using ::fast_io::native_file_loader;

using ::fast_io::in;
using ::fast_io::out;
using ::fast_io::err;
using ::fast_io::u8in;
using ::fast_io::u8out;
using ::fast_io::u8err;

using ::fast_io::posix_stdin;
using ::fast_io::posix_stdout;
using ::fast_io::posix_stderr;

#if(defined(_WIN32) || defined(__CYGWIN__)) || ((!defined(_WIN32) || defined(__WINE__)) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__))))
using ::fast_io::native_dll_file;
using ::fast_io::dll_mode;
using ::fast_io::dll_load_symbol;
#endif

#if defined(__linux__)
using ::fast_io::system_call;
#endif

using ::fast_io::posix_clock_id;
using ::fast_io::posix_clock_gettime;

} // namespace fast_io
