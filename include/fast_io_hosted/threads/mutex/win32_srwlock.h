#pragma once
/*
Referenced from
https://learn.microsoft.com/en-us/archive/msdn-magazine/2012/november/windows-with-c-the-evolution-of-synchronization-in-windows-and-c
*/
namespace fast_io
{

struct win32_srwlock
{
	using native_handle_type = ::fast_io::win32::nt::rtl_srwlock;
	native_handle_type h{.Ptr = nullptr};
	explicit constexpr win32_srwlock() noexcept = default;
	win32_srwlock(win32_srwlock const &) = delete;
	win32_srwlock &operator=(win32_srwlock const &) = delete;
	void lock() noexcept
	{
		::fast_io::win32::AcquireSRWLockExclusive(__builtin_addressof(h));
	}
	bool try_lock() noexcept
	{
		return ::fast_io::win32::TryAcquireSRWLockExclusive(__builtin_addressof(h)) != 0;
	}
	void unlock() noexcept
	{
		::fast_io::win32::ReleaseSRWLockExclusive(__builtin_addressof(h));
	}
};

} // namespace fast_io