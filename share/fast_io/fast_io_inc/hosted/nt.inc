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
        using ::fast_io::win32::nt::nt_query_system_information;
        using ::fast_io::win32::nt::system_information_class;
        using ::fast_io::win32::nt::nt_allocate_virtual_memory;
        using ::fast_io::win32::nt::nt_free_virtual_memory;
        using ::fast_io::win32::nt::system_basic_information;
        using ::fast_io::win32::nt::nt_yield_execution;
        using ::fast_io::win32::nt::file_fs_device_type;
        using ::fast_io::win32::nt::io_status_block;
        using ::fast_io::win32::nt::fs_information_class;
        using ::fast_io::win32::nt::nt_query_volume_information_file;
        using ::fast_io::win32::nt::file_basic_information;
        using ::fast_io::win32::nt::file_information_class;
        using ::fast_io::win32::nt::nt_query_information_file;
        using ::fast_io::win32::nt::nt_create_timer;
        using ::fast_io::win32::nt::nt_set_information_file;
        using ::fast_io::win32::nt::file_allocation_information;
        using ::fast_io::win32::nt::RtlGetSystemTimePrecise;
        using ::fast_io::win32::nt::file_standard_information;
        using ::fast_io::win32::nt::nt_set_timer;
        using ::fast_io::win32::nt::timer_type;
        using ::fast_io::win32::nt::nt_close;
        using ::fast_io::win32::nt::nt_wait_for_multiple_objects;
        using ::fast_io::win32::nt::wait_type;
        using ::fast_io::win32::nt::nt_delay_execution;
        using ::fast_io::win32::nt::nt_wait_for_single_object;
    } // namespace win32::nt
}