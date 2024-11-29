#pragma once

namespace fast_io
{

struct win32_critical_section
{
	using native_handle_type = ::fast_io::win32::nt::rtl_critical_section;
	native_handle_type critical_section;
	inline explicit win32_critical_section() noexcept
	{
		::fast_io::win32::InitializeCriticalSection(__builtin_addressof(critical_section));
	}
	inline win32_critical_section(win32_critical_section const &) = delete;
	inline win32_critical_section &operator=(win32_critical_section const &) = delete;
	inline void lock() noexcept
	{
		::fast_io::win32::EnterCriticalSection(__builtin_addressof(critical_section));
	}
	inline bool try_lock() noexcept
	{
		return ::fast_io::win32::TryEnterCriticalSection(__builtin_addressof(critical_section)) != 0;
	}
	inline void unlock() noexcept
	{
		::fast_io::win32::LeaveCriticalSection(__builtin_addressof(critical_section));
	}
	inline ~win32_critical_section()
	{
		::fast_io::win32::DeleteCriticalSection(__builtin_addressof(critical_section));
	}
};

} // namespace fast_io
