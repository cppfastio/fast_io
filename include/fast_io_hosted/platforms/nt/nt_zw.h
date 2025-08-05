#pragma once

namespace fast_io::win32::nt
{

template <bool zw>
inline ::std::uint_least32_t nt_close(void *handle) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwClose(handle);
	}
	else
	{
		return ::fast_io::win32::nt::NtClose(handle);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_create_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwCreateFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtCreateFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 7)
inline ::std::uint_least32_t nt_create_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwCreateSection(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtCreateSection(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_information_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryInformationProcess(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryInformationProcess(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 9)
inline ::std::uint_least32_t nt_write_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwWriteFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtWriteFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 9)
inline ::std::uint_least32_t nt_read_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwReadFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtReadFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryObject(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryObject(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_query_directory_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryDirectoryFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryDirectoryFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQuerySection(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQuerySection(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_information_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryInformationFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryInformationFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_set_information_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwSetInformationFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtSetInformationFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 7)
inline ::std::uint_least32_t nt_duplicate_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwDuplicateObject(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtDuplicateObject(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_wait_for_single_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwWaitForSingleObject(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtWaitForSingleObject(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_set_system_time(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwSetSystemTime(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtSetSystemTime(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 8)
inline ::std::uint_least32_t nt_create_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwCreateProcess(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtCreateProcess(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_create_user_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwCreateUserProcess(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtCreateUserProcess(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_resume_thread(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwResumeThread(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtResumeThread(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_lock_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwLockFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtLockFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_unlock_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwUnlockFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtUnlockFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_flush_buffers_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwFlushBuffersFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtFlushBuffersFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_flush_buffers_file_ex(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwFlushBuffersFileEx(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtFlushBuffersFileEx(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_map_view_of_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwMapViewOfSection(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtMapViewOfSection(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_unmap_view_of_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwUnmapViewOfSection(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtUnmapViewOfSection(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_read_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwReadVirtualMemory(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtReadVirtualMemory(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_write_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwWriteVirtualMemory(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtWriteVirtualMemory(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_allocate_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAllocateVirtualMemory(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAllocateVirtualMemory(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 4)
inline ::std::uint_least32_t nt_free_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwFreeVirtualMemory(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtFreeVirtualMemory(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 4)
inline ::std::uint_least32_t nt_query_system_information(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQuerySystemInformation(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQuerySystemInformation(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_volume_information_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryVolumeInformationFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryVolumeInformationFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_fs_control_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwFsControlFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtFsControlFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 14)
inline ::std::uint_least32_t nt_create_named_pipe_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwCreateNamedPipeFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtCreateNamedPipeFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_query_performance_counter(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryPerformanceCounter(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryPerformanceCounter(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_information_thread(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryInformationThread(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryInformationThread(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 4)
inline ::std::uint_least32_t nt_set_information_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwSetInformationObject(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtSetInformationObject(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 1)
inline ::std::uint_least32_t nt_query_auxiliary_counter_frequency(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwQueryAuxiliaryCounterFrequency(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtQueryAuxiliaryCounterFrequency(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_create_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcCreatePort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcCreatePort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_alpc_connect_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcConnectPort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcConnectPort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 8)
inline ::std::uint_least32_t nt_alpc_send_wait_receive_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcSendWaitReceivePort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcSendWaitReceivePort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 9)
inline ::std::uint_least32_t nt_alpc_accept_connect_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcAcceptConnectPort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcAcceptConnectPort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_impersonate_client_of_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcImpersonateClientOfPort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcImpersonateClientOfPort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_impersonate_client_container_of_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcImpersonateClientContainerOfPort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcImpersonateClientContainerOfPort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_alpc_disconnect_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcDisconnectPort(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcDisconnectPort(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_alpc_create_port_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcCreatePortSection(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcCreatePortSection(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_create_section_view(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcCreateSectionView(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcCreateSectionView(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_alpc_query_information_message(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcQueryInformationMessage(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcQueryInformationMessage(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_create_security_context(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcCreateSecurityContext(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcCreateSecurityContext(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_alpc_open_sender_thread(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwAlpcOpenSenderThread(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtAlpcOpenSenderThread(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_terminate_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwTerminateProcess(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtTerminateProcess(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_device_io_control_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwDeviceIoControlFile(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtDeviceIoControlFile(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_set_information_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwSetInformationVirtualMemory(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtSetInformationVirtualMemory(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_delay_execution(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwDelayExecution(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtDelayExecution(args...);
	}
}

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_set_timer_resolution(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ::fast_io::win32::nt::ZwSetTimerResolution(args...);
	}
	else
	{
		return ::fast_io::win32::nt::NtSetTimerResolution(args...);
	}
}

} // namespace fast_io::win32::nt
