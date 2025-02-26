#pragma once

namespace fast_io
{
/*
*  UNFINISHED, UNAUDITED !!!!!!!!
*/
/*
	system calls

	01;0000001 /exit
	01;0000002 /fork
	01;0000003 /read
	01;0000004 /write
	01;0000005 /open
	01;0000006 /close
	01;0000007 /wait
	01;0000010 /creat
	01;0000011 /link
	01;0000012 /unlink
	01;0000013 /exec
	01;0000014 /chdir
	01;0000015 /time
	01;0000016 /makdir
	01;0000017 /chmod
	01;0000020 /chown
	01;0000021 /break
	01;0000022 /stat
	01;0000023 /seek
	01;0000024 /tell
	01;0000025 /mount
	01;0000026 /umount
	01;0000027 /setuid
	01;0000030 /getuid
	01;0000031 /stime
	01;0000034 /fstat
	01;0000036 /mdate
	01;0000037 /stty
	01;0000040 /gtty
	01;0000042 /nice
	01;0000060 /signal
*/
namespace unix
{
template <::std::size_t syscall_number, ::std::signed_integral return_value_type>
	requires(1 < sizeof(return_value_type))
inline return_value_type system_call() noexcept
{
	return_value_type ret;
	__asm__ __volatile__(
		"mov %1,r0\n\t"
		"trap 0\n\t" 
		"mov r0,%0\n\t"
		: "=r"(ret)
		: "r"(syscall_number)
		: "memory", "cc"
	);
	return ret;
}

template <::std::size_t syscall_number, ::std::signed_integral return_value_type>
	requires(1 < sizeof(return_value_type))
inline return_value_type system_call(auto p1) noexcept
{
	return_value_type ret;
	__asm__ __volatile__(
		"mov %1,r0\n\t"
		"mov %2,r1\n\t"
		"trap 0\n\t"
		"mov r0,%0\n\t"
		: "=r"(ret)
		: "r"(syscall_number), "r"(p1)
		: "memory", "cc");
	return ret;
}

template <::std::size_t syscall_number>
inline void system_call_no_return(auto p1) noexcept
{
	__asm__ __volatile__(
		"mov %0,r0\n\t"
		"mov %1,r1\n\t"
		"trap 0\n\t"
		: 
		: "r"(syscall_number), "r"(p1)
		: "memory", "cc");
	__builtin_unreachable();
}

template <::std::size_t syscall_number, ::std::signed_integral return_value_type>
	requires(1 < sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2) noexcept
{
	return_value_type ret;
	__asm__ __volatile__(
		"mov %1,r0\n\t"
		"mov %2,r1\n\t"
		"mov %3,r2\n\t"
		"trap 0\n\t"
		"mov r0,%0\n\t"
		: "=r"(ret)
		: "r"(syscall_number), "r"(p1), "r"(p2)
		: "memory", "cc");
	return ret;
}

template <::std::size_t syscall_number, ::std::signed_integral return_value_type>
	requires(1 < sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2, auto p3) noexcept
{
	return_value_type ret;
	__asm__ __volatile__(
		"mov %1,r0\n\t"
		"mov %2,r1\n\t"
		"mov %3,r2\n\t"
		"mov %4,r3\n\t"
		"trap 0\n\t"
		"mov r0,%0\n\t"
		: "=r"(ret)
		: "r"(syscall_number), "r"(p1), "r"(p2), "r"(p3)
		: "memory", "cc");
	return ret;
}
} // namespace unix
} // namespace fast_io
