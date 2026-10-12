export namespace fast_io
{
    using ::fast_io::win32_stdout_number;
    using ::fast_io::win32_stderr_number;
        
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
        using ::fast_io::win32::GetSystemInfo;
        using ::fast_io::win32::system_info;
        using ::fast_io::win32::VirtualAlloc;
        using ::fast_io::win32::VirtualFree;
        using ::fast_io::win32::GetEnvironmentStringsW;
        using ::fast_io::win32::GetEnvironmentStringsA;
        using ::fast_io::win32::FreeEnvironmentStringsW;
        using ::fast_io::win32::FreeEnvironmentStringsA;
        using ::fast_io::win32::AddVectoredExceptionHandler;
        using ::fast_io::win32::exception_pointers;
        using ::fast_io::win32::RaiseException;
        using ::fast_io::win32::getsockopt;
        using ::fast_io::win32::ioctlsocket;
        using ::fast_io::win32::WSAGetLastError;
        using ::fast_io::win32::shutdown;
        using ::fast_io::win32::ExitProcess;
        using ::fast_io::win32::wsabuf;
        using ::fast_io::win32::WSARecv;
        using ::fast_io::win32::WSASend;
        using ::fast_io::win32::GetFileType;
        using ::fast_io::win32::sockaddr_in;
        using ::fast_io::win32::sockaddr_storage;
        using ::fast_io::win32::sockaddr_in6;
        using ::fast_io::win32::WSACreateEvent;
        using ::fast_io::win32::WSACloseEvent;
        using ::fast_io::win32::WSAEventSelect;
        using ::fast_io::win32::wsanetworkevents;
        using ::fast_io::win32::WSAEnumNetworkEvents;

    } // namespace win32

    using ::fast_io::win32_socket_file;
    using ::fast_io::win32_socket_io_observer;
    using ::fast_io::win32_socket_event_guard_t;

    using ::fast_io::data_sync_flags;
    using ::fast_io::data_sync;
    using ::fast_io::flush;
}
