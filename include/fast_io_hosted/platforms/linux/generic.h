#pragma once

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <unistd.h>
#include <sys/syscall.h>

namespace fast_io
{

template <::std::size_t syscall_number, ::std::signed_integral return_value_type, typename... Args>
	requires(::std::is_trivially_copyable_v<Args> && ...)
inline return_value_type system_call(Args... args) noexcept
{
	long ret{::syscall(syscall_number, args...)};
	return static_cast<return_value_type>(ret);
}

template <::std::uint_least64_t syscall_number, ::std::signed_integral return_value_type>
	requires(1 < sizeof(return_value_type))
[[__gnu__::__always_inline__]]
inline return_value_type inline_syscall(auto p1, auto p2) noexcept
{
	static_assert(false, "force inline for generic syscall is not supported");
}

template <::std::size_t syscall_number>
[[noreturn]]
inline void system_call_no_return(auto p1) noexcept
{
	::syscall(syscall_number, p1);
	__builtin_unreachable();
}

template <::std::integral I>
[[noreturn]]
inline void fast_exit(I ret) noexcept
{
	system_call_no_return<__NR_exit>(ret);
}
} // namespace fast_io
