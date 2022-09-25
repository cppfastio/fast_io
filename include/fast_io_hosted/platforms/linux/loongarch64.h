#pragma once
/*
Referenced from
glibc loongarch
loongarch syscall is very similar to riscv. but it is using syscall instruction like x86_64
*/

namespace fast_io
{

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call() noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0");
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::uint_least64_t syscall_number>
inline void system_call_no_return(auto p1) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7)
	: "memory"
	);
}

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	register std::uint_least64_t a1 __asm__("$a1") = (std::uint_least64_t)p2;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7), "r"(a1)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	register std::uint_least64_t a1 __asm__("$a1") = (std::uint_least64_t)p2;
	register std::uint_least64_t a2 __asm__("$a2") = (std::uint_least64_t)p3;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7), "r"(a1), "r"(a2)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3,auto p4) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	register std::uint_least64_t a1 __asm__("$a1") = (std::uint_least64_t)p2;
	register std::uint_least64_t a2 __asm__("$a2") = (std::uint_least64_t)p3;
	register std::uint_least64_t a3 __asm__("$a3") = (std::uint_least64_t)p4;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7), "r"(a1), "r"(a2), "r"(a3)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3,auto p4,auto p5) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	register std::uint_least64_t a1 __asm__("$a1") = (std::uint_least64_t)p2;
	register std::uint_least64_t a2 __asm__("$a2") = (std::uint_least64_t)p3;
	register std::uint_least64_t a3 __asm__("$a3") = (std::uint_least64_t)p4;
	register std::uint_least64_t a4 __asm__("$a4") = (std::uint_least64_t)p5;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7), "r"(a1), "r"(a2), "r"(a3), "r"(a4)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::uint_least64_t syscall_number,std::signed_integral return_value_type>
requires (1<sizeof(return_value_type))
inline return_value_type system_call(auto p1,auto p2,auto p3,auto p4,auto p5,auto p6) noexcept
{
	register std::uint_least64_t a7 __asm__("$a7") = syscall_number;
	register std::uint_least64_t a0 __asm__("$a0") = (std::uint_least64_t)p1;
	register std::uint_least64_t a1 __asm__("$a1") = (std::uint_least64_t)p2;
	register std::uint_least64_t a2 __asm__("$a2") = (std::uint_least64_t)p3;
	register std::uint_least64_t a3 __asm__("$a3") = (std::uint_least64_t)p4;
	register std::uint_least64_t a4 __asm__("$a4") = (std::uint_least64_t)p5;
	register std::uint_least64_t a5 __asm__("$a5") = (std::uint_least64_t)p6;
	__asm__ __volatile__
	("syscall 0"
	: "+r" (a0)
	: "r"(a7), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5)
	: "memory"
	);
	return static_cast<return_value_type>(a0);
}

template<std::integral I>
inline void fast_exit(I ret) noexcept
{
	system_call_no_return<__NR_exit>(ret);
}

}
