export namespace fast_io
{

#if defined(_WIN32) || defined(__CYGWIN__)
using ::fast_io::win32_family;
using ::fast_io::nt_family;

using ::fast_io::basic_nt_family_io_observer;
using ::fast_io::basic_nt_family_file;
using ::fast_io::nt_io_observer;
using ::fast_io::nt_file;
using ::fast_io::basic_win32_family_io_observer;
using ::fast_io::basic_win32_family_file;
using ::fast_io::win32_io_observer;
using ::fast_io::win32_file;
#endif

using ::fast_io::basic_posix_family_io_observer;
using ::fast_io::basic_posix_family_file;
using ::fast_io::posix_io_observer;
using ::fast_io::posix_file;
using ::fast_io::native_file;
using ::fast_io::native_io_observer;

using ::fast_io::at;
using ::fast_io::drt;
using ::fast_io::current;
using ::fast_io::recursive;

using ::fast_io::native_file_loader;

} // namespace fast_io
