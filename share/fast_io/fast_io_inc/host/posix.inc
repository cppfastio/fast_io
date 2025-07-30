export namespace fast_io
{
    using ::fast_io::posix_tzset;
    using ::fast_io::posix_stdin_number;
    using ::fast_io::posix_stdout_number;
    using ::fast_io::posix_stderr_number;

    namespace posix
    {
        using ::fast_io::posix::libc_ioctl;
    }
}