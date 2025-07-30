export namespace fast_io
{
    using ::fast_io::win32_stdout_number;
    using ::fast_io::win32_stderr_number;
    using ::fast_io::win32_domain_value;
    
    namespace win32
    {
        using ::fast_io::win32::GetLastError;
        using ::fast_io::win32::GetStdHandle;
        using ::fast_io::win32::GetConsoleMode;
        using ::fast_io::win32::SetConsoleMode;
        using ::fast_io::win32::GetConsoleCP;
        using ::fast_io::win32::GetConsoleOutputCP;
        using ::fast_io::win32::SetConsoleCP;
        using ::fast_io::win32::SetConsoleOutputCP;
        using ::fast_io::win32::CommandLineToArgvW;
        using ::fast_io::win32::LocalFree;
        using ::fast_io::win32::GetEnvironmentVariableW;
        using ::fast_io::win32::GetEnvironmentVariableA;
        using ::fast_io::win32::PrefetchVirtualMemory;
        using ::fast_io::win32::win32_memory_range_entry;
        using ::fast_io::win32::SetConsoleTextAttribute;
        using ::fast_io::win32::console_screen_buffer_info;
        using ::fast_io::win32::GetConsoleScreenBufferInfo;
    } // namespace win32
}