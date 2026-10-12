export namespace fast_io
{
    using ::fast_io::posix_tzset;
    using ::fast_io::posix_stdin_number;
    using ::fast_io::posix_stdout_number;
    using ::fast_io::posix_stderr_number;
    
#if ((!defined(_WIN32) || defined(__WINE__)) || defined(__CYGWIN__)) && !defined(__wasm__)
    namespace posix
    {
        using ::fast_io::posix::libc_ioctl;
    }

    namespace details
    {
        using ::fast_io::details::sys_mmap;
        using ::fast_io::details::sys_mprotect;
        using ::fast_io::details::sys_munmap;
        using ::fast_io::details::sys_munmap_nothrow;
    }

    using ::fast_io::posix_socklen_t;

#endif
}