#pragma once

namespace fast_io
{
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
inline void exit(int status) noexcept
{
	__asm__ __volatile__(
		"mov %0, r2\n\t"
		"sys 1\n\t"
		:
		: "r"(status)
		: "memory", "cc");
}

inline int write(int fd, char const *buf, int len) noexcept
{
	int ret;
	__asm__ __volatile__(
		"mov %1, r2\n\t"
		"mov %2, r3\n\t"
		"mov %3, r4\n\t"
		"sys 4\n\t"
		"mov r0, %0\n\t"
		: "=r"(ret)
		: "r"(fd), "r"(buf), "r"(len)
		: "memory", "cc");
	return ret;
}
} // namespace unix
} // namespace fast_io
