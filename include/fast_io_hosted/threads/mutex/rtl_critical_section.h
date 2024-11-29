#pragma once

namespace fast_io
{

struct rtl_critical_section
{
	using native_handle_type = ::fast_io::win32::nt::rtl_critical_section;
	native_handle_type critical_section;
	inline explicit rtl_critical_section() noexcept
	{
		::fast_io::win32::nt::RtlInitializeCriticalSection(__builtin_addressof(critical_section));
	}
	inline rtl_critical_section(rtl_critical_section const &) = delete;
	inline rtl_critical_section &operator=(rtl_critical_section const &) = delete;
	inline void lock() noexcept
	{
		::fast_io::win32::nt::RtlEnterCriticalSection(__builtin_addressof(critical_section));
	}
	inline bool try_lock() noexcept
	{
		return ::fast_io::win32::nt::RtlTryEnterCriticalSection(__builtin_addressof(critical_section)) != 0;
	}
	inline void unlock() noexcept
	{
		::fast_io::win32::nt::RtlLeaveCriticalSection(__builtin_addressof(critical_section));
	}
	inline ~rtl_critical_section()
	{
		::fast_io::win32::nt::RtlDeleteCriticalSection(__builtin_addressof(critical_section));
	}
};

} // namespace fast_io
