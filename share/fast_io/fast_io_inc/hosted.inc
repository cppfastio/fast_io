export namespace fast_io
{

#if defined(_WIN32) || defined(__CYGWIN__)
using ::fast_io::win32_family;
using ::fast_io::nt_family;
#endif

} // namespace fast_io
