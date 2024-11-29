#pragma once
/*
Referenced from
https://learn.microsoft.com/en-us/archive/msdn-magazine/2012/november/windows-with-c-the-evolution-of-synchronization-in-windows-and-c
*/
namespace fast_io
{

struct rtl_srwlock
{
	using native_handle_type = ::fast_io::win32::nt::rtl_srwlock;
	native_handle_type h{.Ptr = nullptr};
	inline explicit constexpr rtl_srwlock() noexcept = default;
	inline rtl_srwlock(rtl_srwlock const &) = delete;
	inline rtl_srwlock &operator=(rtl_srwlock const &) = delete;
	inline void lock() noexcept
	{
		::fast_io::win32::nt::RtlAcquireSRWLockExclusive(__builtin_addressof(h));
	}
	inline bool try_lock() noexcept
	{
		return ::fast_io::win32::nt::RtlTryAcquireSRWLockExclusive(__builtin_addressof(h)) != 0;
	}
	inline void unlock() noexcept
	{
		::fast_io::win32::nt::RtlReleaseSRWLockExclusive(__builtin_addressof(h));
	}
};

} // namespace fast_io
