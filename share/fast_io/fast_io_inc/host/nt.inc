export namespace fast_io
{

    namespace win32::nt
    {
        using ::fast_io::win32::nt::nt_get_current_peb;
        using ::fast_io::win32::nt::CsrClientCallServer;
        using ::fast_io::win32::nt::RtlQueryEnvironmentVariable_U;
        using ::fast_io::win32::nt::unicode_string;
        using ::fast_io::win32::nt::nt_set_information_virtual_memory;
        using ::fast_io::win32::nt::memory_range_entry;
        using ::fast_io::win32::nt::virtual_memory_information_class;
    } // namespace win32::nt
}