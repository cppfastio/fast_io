#pragma once
//https://github.com/bminor/glibc/master/sysdeps/unix/sysv/linux/x86_64/sysdep.h

namespace fast_io
{

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call() noexcept
{
	return_value_type ret;
	__asm__ __volatile__
	(
	"syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1) noexcept
{
	return_value_type ret;
	__asm__ __volatile__
	(
	"syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}

template<std::size_t syscall_number>
[[noreturn]]
inline void system_call_no_return(auto p1) noexcept
{
	std::size_t ret;
	__asm__ __volatile__
	(
	"syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1)
	: "memory", "cc", "r11", "cx"
	);
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2) noexcept
{
	return_value_type ret;
	__asm__ __volatile__
	(
	"syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1), "S"(p2)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2, auto p3) noexcept
{
	return_value_type ret;
	__asm__ __volatile__
	(
	"syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1), "S"(p2), "d"(p3)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2, auto p3, auto p4) noexcept
{
	return_value_type ret;
	register std::uint_least64_t r10 __asm__("r10") = (std::uint_least64_t)p4;
	__asm__ __volatile__
	("syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1), "S"(p2), "d"(p3), "r"(r10)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2, auto p3, auto p4,auto p5) noexcept
{
	return_value_type ret;
	register std::uint_least64_t r10 __asm__("r10") = (std::uint_least64_t)p4;
	register std::uint_least64_t r8 __asm__("r8") = (std::uint_least64_t)p5;
	__asm__ __volatile__
	("syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1), "S"(p2), "d"(p3), "r"(r10), "r"(r8)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}

template<std::size_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1, auto p2, auto p3, auto p4,auto p5,auto p6) noexcept
{
	return_value_type ret;
	register std::uint_least64_t r10 __asm__("r10") = (std::uint_least64_t)p4;
	register std::uint_least64_t r8 __asm__("r8") = (std::uint_least64_t)p5;
	register std::uint_least64_t r9 __asm__("r9") = (std::uint_least64_t)p6;
	__asm__ __volatile__
	("syscall"
	: "=a" (ret)
	//EDI      RSI       RDX
	: "0"(syscall_number), "D"(p1), "S"(p2), "d"(p3), "r"(r10), "r"(r8), "r"(r9)
	: "memory", "cc", "r11", "cx"
	);
	return ret;
}


template<std::integral I>
inline void fast_exit(I ret) noexcept
{
	system_call_no_return<__NR_exit>(ret);
}
}
